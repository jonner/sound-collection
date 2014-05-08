/*
 * application.cc
 * This file is part of SoundCollection
 *
 * Copyright (C) 2014 - Jonathon Jongsma
 *
 * SoundCollection is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SoundCollection is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SoundCollection. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gio/gio.h>
#include <gom/gom.h>
#include <gst/gst.h>

#include "application.h"
#include "equipment-resource.h"
#include "GRefPtr.h"
#include "identification-resource.h"
#include "location-resource.h"
#include "main-window.h"
#include "recording.h"
#include "recording-resource.h"
#include "species-resource.h"
#include "task.h"

#define REPOSITORY_VERSION 1

namespace SC {

struct Application::Priv {
    Priv()
        : status(0)
    {
    }

    int status;
    Glib::RefPtr<Gio::File> db;
    WTF::GRefPtr<GomAdapter> adapter;
    WTF::GRefPtr<GomRepository> repository;
    MainWindow window;
};

Glib::RefPtr<Application> Application::create()
{
    return Glib::RefPtr<Application>(new Application());
}

Application::Application()
    : Gtk::Application("org.quotidian.SoundCollection",
                       Gio::APPLICATION_HANDLES_COMMAND_LINE)
    , m_priv(new Priv())
{
}

int Application::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
    int argc = 0;
    char** argv = command_line->get_arguments(argc);
    gst_init(NULL, NULL);
    g_debug("Got commandline: %i elements", argc);

    g_debug("argc = %i", argc);
    if (argc > 1)
        m_priv->db = command_line->create_file_for_arg(argv[1]);
    else
        m_priv->db = command_line->create_file_for_arg("sound-collection.sqlite");

    std::string uri = m_priv->db->get_uri();
    g_debug("Opening db %s...", uri.c_str());

    m_priv->adapter = adoptGRef(gom_adapter_new());
    gom_adapter_open_async(m_priv->adapter.get(),
                           uri.c_str(),
                           Application::adapter_open_ready_proxy,
                           this);

    g_strfreev(argv);

    add_window(m_priv->window);
    return 0;
}

static GType repository_types[] = { SC_TYPE_RECORDING_RESOURCE,
                                    SC_TYPE_SPECIES_RESOURCE,
                                    SC_TYPE_IDENTIFICATION_RESOURCE,
                                    SC_TYPE_LOCATION_RESOURCE,
                                    SC_TYPE_EQUIPMENT_RESOURCE };

void Application::adapter_open_ready(GomAdapter* adapter, GAsyncResult* res)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    if (!gom_adapter_open_finish(adapter, res, &error)) {
        g_warning("failed to open adapter: %s", error->message);
        g_error_free(error);
        m_priv->status = -1;
        quit();
    } else
        g_debug("Opened adapter");

    m_priv->repository = adoptGRef(gom_repository_new(m_priv->adapter.get()));
    GList* types = 0;
    for (int i = 0; i < G_N_ELEMENTS(repository_types); i++) {
        types = g_list_prepend(types, GINT_TO_POINTER(repository_types[i]));
    }
    gom_repository_automatic_migrate_async(
        m_priv->repository.get(),
        REPOSITORY_VERSION,
        types,
        Application::repository_migrate_finished_proxy,
        this);
}

void Application::adapter_open_ready_proxy(GObject* source_object,
                                           GAsyncResult* res,
                                           gpointer user_data)
{
    Application* self = static_cast<Application*>(user_data);
    GomAdapter* adapter = reinterpret_cast<GomAdapter*>(source_object);

    self->adapter_open_ready(adapter, res);
}

void Application::repository_migrate_finished_proxy(GObject* source_object,
                                                    GAsyncResult* res,
                                                    gpointer user_data)
{
    Application* self = static_cast<Application*>(user_data);
    GomRepository* repository = reinterpret_cast<GomRepository*>(source_object);
    self->repository_migrate_finished(repository, res);
}

void Application::repository_migrate_finished(GomRepository* repository,
                                              GAsyncResult* res)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    if (!gom_repository_automatic_migrate_finish(repository, res, &error)) {
        g_warning("failed to migrate repository: %s", error->message);
        g_error_free(error);
        m_priv->status = -1;
        quit();
    } else
        g_debug("Repository migrated");

    show();
}

void Application::show()
{
    m_priv->window.set_repository(m_priv->repository.get());
    m_priv->window.show();
}

Glib::RefPtr<const Gio::File> Application::database() const
{
    return m_priv->db;
}

struct ImportFileTask : public Task {
    Application* application;
    std::tr1::shared_ptr<Recording> recording;

    ImportFileTask(Application* app, const Gio::SlotAsyncReady& slot)
        : Task(slot)
        , application(app)
    {
    }
};

bool Application::import_file_finish(const Glib::RefPtr<Gio::AsyncResult>& result)
{
    GTask* task = G_TASK(result->gobj());
    GError* error = 0;
    bool status = g_task_propagate_boolean(task, &error);
    if (error)
        throw Glib::Error(error);

    return status;
}

void resource_save_ready_proxy(GObject* source,
                               GAsyncResult* result,
                               gpointer user_data)
{
    ImportFileTask* task = reinterpret_cast<ImportFileTask*>(user_data);
    GError* error = 0;
    GomResource* resource = GOM_RESOURCE(source);
    if (!gom_resource_save_finish(resource, result, &error)) {
        g_task_return_error(task->task(), error);
        return;
    }

    g_debug("saved resource");
    g_task_return_boolean(task->task(), true);
}

void on_calculate_duration_ready(const Glib::RefPtr<Gio::AsyncResult>& result,
                                 ImportFileTask* task)
{
    float duration;
    try
    {
        task->recording->calculate_duration_finish(result, duration);
    }
    catch (const Glib::Error& error)
    {
        GError* gerror = g_error_copy(error.gobj());
        g_task_return_error(task->task(), gerror);
        return;
    }

    g_debug("Got duration for file: %g", duration);
    g_object_set(task->recording->resource(),
                 "duration",
                 duration,
                 NULL);
    g_debug("saving...");
    gom_resource_save_async(GOM_RESOURCE(task->recording->resource()),
                            resource_save_ready_proxy,
                            task);
}

void Application::import_file_async(const Glib::RefPtr<Gio::File>& file,
                                    const Gio::SlotAsyncReady& slot)
{
    ImportFileTask* task = new ImportFileTask(this, slot);
    if (file->query_file_type() != Gio::FILE_TYPE_REGULAR) {
        g_task_return_new_error(task->task(),
                                G_FILE_ERROR,
                                G_FILE_ERROR_EXIST,
                                "File doesn't exist or is not a regular file");
        return;
    }

    g_debug("Importing file %s", file->get_path().c_str());
    WTF::GRefPtr<ScRecordingResource> resource = adoptGRef(SC_RECORDING_RESOURCE(
        g_object_new(SC_TYPE_RECORDING_RESOURCE,
                     "repository",
                     m_priv->repository.get(),
                     "file",
                     file->get_path().c_str(),
                     NULL)));
    task->recording = Recording::create(resource.get());
    task->recording->calculate_duration_async(
        sigc::bind(sigc::ptr_fun(on_calculate_duration_ready), task));
}
}
