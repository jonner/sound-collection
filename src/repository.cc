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
#include "equipment-resource.h"
#include "GRefPtr.h"
#include "identification-resource.h"
#include "location-resource.h"
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

    Priv(GomAdapter* adapter)
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

Repository::Repository(GomAdapter* adapter)
    : m_priv(new Priv(adapter))
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

}
