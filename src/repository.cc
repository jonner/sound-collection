/*
 * repository.cc
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

#include <gom/gom.h>
#include <iomanip>

#include "equipment-resource.h"
#include "GRefPtr.h"
#include "identification-resource.h"
#include "location-resource.h"
#include "recording.h"
#include "recording-resource.h"
#include "recording-resource.h"
#include "repository.h"
#include "species-resource.h"
#include "task.h"

namespace SC {

static GType repository_types[] = { SC_TYPE_RECORDING_RESOURCE,
                                    SC_TYPE_SPECIES_RESOURCE,
                                    SC_TYPE_IDENTIFICATION_RESOURCE,
                                    SC_TYPE_LOCATION_RESOURCE,
                                    SC_TYPE_EQUIPMENT_RESOURCE };

#define REPOSITORY_VERSION 1

struct Repository::Priv {
    WTF::GRefPtr<GomRepository> repository;
    mutable sigc::signal<void> signal_database_changed;
    Glib::RefPtr<Gio::File> audio_dir;

    Priv(GomAdapter* adapter, const Glib::ustring& audio_path)
        : audio_dir(Gio::File::create_for_path(audio_path))
    {
        repository = adoptGRef(gom_repository_new(adapter));

        GList* types = 0;
        for (int i = 0; i < G_N_ELEMENTS(repository_types); i++) {
            types = g_list_prepend(types, GINT_TO_POINTER(repository_types[i]));
        }
        gom_repository_automatic_migrate_async(
            repository.get(),
            REPOSITORY_VERSION,
            types,
            Repository::repository_migrate_finished_proxy,
            this);
    }
};

Repository::Repository(GomAdapter* adapter,
                       const Glib::ustring& audio_path)
    : m_priv(new Priv(adapter, audio_path))
{
}

GomRepository* Repository::cobj()
{
    return m_priv->repository.get();
}

struct GetLocationsTask : public Task {
    Repository* repository;

    GetLocationsTask(Repository* repository, const Gio::SlotAsyncReady& slot)
        : Task(slot)
        , repository(repository)
    {
    }
};

void found_locations(GObject* src,
                     GAsyncResult* result,
                     gpointer user_data)
{
    GError* error = 0;
    GetLocationsTask* task = reinterpret_cast<GetLocationsTask*>(user_data);
    GomResourceGroup* resources = gom_repository_find_finish(GOM_REPOSITORY(src),
                                                             result,
                                                             &error);
    if (error) {
        g_warning("Couldn't find locations: %s", error->message);
        g_task_return_error(task->task(), error);
        return;
    }

    g_task_return_pointer(task->task(), resources, g_object_unref);
}

void Repository::get_locations_async(const Gio::SlotAsyncReady& slot)
{
    GetLocationsTask* task = new GetLocationsTask(this, slot);
    gom_repository_find_async(m_priv->repository.get(),
                              SC_TYPE_LOCATION_RESOURCE,
                              NULL,
                              found_locations,
                              task);
}

GomResourceGroup* Repository::get_locations_finish(const Glib::RefPtr<Gio::AsyncResult>& result)
{
    GTask* task = G_TASK(result->gobj());
    GError* error = 0;
    gpointer resources = g_task_propagate_pointer(task, &error);
    if (error)
        throw Glib::Error(error);

    return GOM_RESOURCE_GROUP(resources);
}

void Repository::repository_migrate_finished_proxy(GObject* source_object,
                                                   GAsyncResult* res,
                                                   gpointer user_data)
{
    Repository* self = static_cast<Repository*>(user_data);
    GomRepository* repository = reinterpret_cast<GomRepository*>(source_object);
    self->repository_migrate_finished(repository, res);
}

void Repository::repository_migrate_finished(GomRepository* repository,
                                             GAsyncResult* res)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    if (!gom_repository_automatic_migrate_finish(repository, res, &error)) {
        g_error("failed to migrate repository: %s", error->message);
        g_error_free(error);
    } else
        g_debug("Repository migrated");
}

sigc::signal<void>& Repository::signal_database_changed() const
{
    return m_priv->signal_database_changed;
}

struct ImportFileTask : public Task {
    Repository* repository;
    std::tr1::shared_ptr<Recording> recording;
    Glib::RefPtr<Gio::File> destfile;

    ImportFileTask(Repository* repository, const Gio::SlotAsyncReady& slot)
        : Task(slot)
        , repository(repository)
    {
    }
};

bool Repository::import_file_finish(const Glib::RefPtr<Gio::AsyncResult>& result)
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
                                                 Glib::ustring::format(std::setfill(L'0'), std::setw(6), task->recording->id()),
                                                 extension);

    g_debug("saved recording %i to database", task->recording->id());

    task->destfile = task->repository->audio_dir()->get_child(newfile);
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

void Repository::import_file_async(const Glib::RefPtr<Gio::File>& file,
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

Glib::RefPtr<Gio::File> Repository::audio_dir() const
{
    return m_priv->audio_dir;
}
}
