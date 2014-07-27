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
#include <iomanip>

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

namespace SC {

const char* DB_NAME = "sound-collection.sqlite";
const char* AUDIO_DIR = "audio";

struct Application::Priv {
    int status;
    Glib::RefPtr<Gio::File> base;
    WTF::GRefPtr<GomAdapter> adapter;
    MainWindow window;
    mutable sigc::signal<void> signal_database_changed;
    std::tr1::shared_ptr<Repository> repository;

    Priv()
        : status(0)
    {
    }

    void setup_collection_directory(const std::string& base_path)
    {
        base = Gio::File::create_for_path(base_path);
        try
        {
            base->make_directory_with_parents();
            base->get_child(AUDIO_DIR)->make_directory();
        }
        catch (const Gio::Error& e)
        {
            if (!(e.domain() == G_IO_ERROR && e.code() == Gio::Error::EXISTS))
                g_error("Unable to setup collection directory: %s(%i) - %s", g_quark_to_string(e.domain()), e.code(), e.what().c_str());
        }

        if (base->query_file_type() != Gio::FILE_TYPE_DIRECTORY)
            g_error("Collection path %s is not a directory", base->get_path().c_str());
    }
};

Glib::RefPtr<Application> Application::create()
{
    return Glib::RefPtr<Application>(new Application());
}

Application::Application()
    : Gtk::Application("org.quotidian.SoundCollection",
                       Gio::APPLICATION_FLAGS_NONE)
    , m_priv(new Priv())
{
}

void Application::on_startup()
{
    Gio::Application::on_startup();
    gst_init(NULL, NULL);
    std::string collection_path = Glib::getenv("COLLECTION_BASE");
    if (collection_path.empty())
        collection_path = Glib::build_filename(Glib::get_user_data_dir(), "SoundCollection");
    m_priv->setup_collection_directory(collection_path);

    std::string uri = database()->get_uri();
    g_debug("Opening db %s...", uri.c_str());

    m_priv->adapter = adoptGRef(gom_adapter_new());
    gom_adapter_open_async(m_priv->adapter.get(),
                           uri.c_str(),
                           Application::adapter_open_ready_proxy,
                           this);
}

void Application::on_activate()
{
    add_window(m_priv->window);
}

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

    m_priv->repository.reset(new Repository(m_priv->adapter.get()));
    show();
}

void Application::adapter_open_ready_proxy(GObject* source_object,
                                           GAsyncResult* res,
                                           gpointer user_data)
{
    Application* self = static_cast<Application*>(user_data);
    GomAdapter* adapter = reinterpret_cast<GomAdapter*>(source_object);

    self->adapter_open_ready(adapter, res);
}


void Application::show()
{
    m_priv->window.set_repository(m_priv->repository);
    m_priv->window.show();
}

Glib::RefPtr<const Gio::File> Application::base() const
{
    return m_priv->base;
}

Glib::RefPtr<const Gio::File> Application::database() const
{
    g_return_val_if_fail(m_priv->base, Glib::RefPtr<const Gio::File>());
    return m_priv->base->get_child(DB_NAME);
}

sigc::signal<void>& Application::signal_database_changed() const
{
    return m_priv->signal_database_changed;
}

struct ImportFileTask : public Task {
    Application* application;
    std::tr1::shared_ptr<Recording> recording;
    Glib::RefPtr<Gio::File> destfile;

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

    signal_database_changed().emit();
    return status;
}

void resource_save_again_ready_proxy(GObject* source,
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
    g_debug("updated resource %i", task->recording->id());
    g_task_return_boolean(task->task(), true);
}

void on_file_copy_ready(const Glib::RefPtr<Gio::AsyncResult>& result,
                        const Glib::RefPtr<Gio::File>& f,
                        ImportFileTask* task)
{
    try
    {
        if (!f->copy_finish(result))
            g_warning("copy_finish failed");
    }
    catch (const Glib::Error& e)
    {
        g_warning("Unable to copy file: %s", e.what().c_str());
        // copying file failed after we already inserted into the DB, so remove it
        gom_resource_delete_async(GOM_RESOURCE(task->recording->resource()), 0, 0);
    }

    g_debug("copied file to %s", f->get_path().c_str());
    g_object_set(task->recording->resource(),
                 "file",
                 task->destfile->get_path().c_str(),
                 "remarks",
                 f->get_path().c_str(),
                 NULL);
    gom_resource_save_async(GOM_RESOURCE(task->recording->resource()),
                            resource_save_again_ready_proxy,
                            task);
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

    std::string filename = task->recording->file()->get_path();
    std::string extension;
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos)
        extension = filename.substr(pos);

    std::string newfile = Glib::ustring::compose("SC%1%2",
                                                 Glib::ustring::format(std::setfill(L'0'), std::setw(10), task->recording->id()),
                                                 extension);

    g_debug("saved recording %i to database", task->recording->id());

    task->destfile = task->application->base()->get_child(AUDIO_DIR)->get_child(newfile);
    Glib::RefPtr<Gio::File> f = task->recording->file();
    f->copy_async(task->destfile, sigc::bind(sigc::bind(sigc::ptr_fun(on_file_copy_ready), task), f), Gio::FILE_COPY_BACKUP);
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
                     "recordist",
                     Glib::get_real_name().c_str(),
                     NULL)));
    task->recording = Recording::create(resource.get());
    task->recording->calculate_duration_async(
        sigc::bind(sigc::ptr_fun(on_calculate_duration_ready), task));
}
}
