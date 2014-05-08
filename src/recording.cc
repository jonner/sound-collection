/*
 * recording.cc
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

#include <gst/gst.h>
#include "GRefPtr.h"
#include "recording.h"

namespace SC {

static Glib::ustring make_string(const char* cstr)
{
    if (cstr)
        return Glib::ustring(cstr);
    return "";
}

struct Recording::Priv {
    WTF::GRefPtr<ScRecordingResource> resource;

    struct QueryDurationData {
        Recording::Priv* priv;
        Gio::SlotAsyncReady slot;
        GstElement* playbin;
        GstBus* bus;
        float duration;
        GTask* task; // weak ref

        QueryDurationData(Priv* priv)
            : priv(priv)
            , playbin(0)
            , bus(0)
            , duration(0.0)
        {
        }

        ~QueryDurationData()
        {
            if (playbin)
                gst_object_unref(playbin);
            if (bus)
                gst_object_unref(bus);
        }

        bool try_update_duration()
        {
            gint64 gstduration = GST_CLOCK_TIME_NONE;
            GstState state = GST_STATE_NULL;
            if (gst_element_get_state(playbin, &state, NULL, GST_CLOCK_TIME_NONE) == GST_STATE_CHANGE_SUCCESS
                && state == GST_STATE_PAUSED) {
                if (gst_element_query_duration(playbin, GST_FORMAT_TIME, &gstduration)) {
                    duration = static_cast<float>(GST_TIME_AS_MSECONDS(gstduration)) / 1000.0;
                    g_task_return_boolean(task, true);
                    return true;
                }
            }
            return false;
        }

        static void async_ready_proxy(GObject* source,
                                      GAsyncResult* result,
                                      gpointer user_data)
        {
            QueryDurationData* data = reinterpret_cast<QueryDurationData*>(user_data);
            Glib::RefPtr<Gio::AsyncResult> cpptype = Glib::wrap(result);
            data->slot(cpptype);
        }
    };

    Priv(ScRecordingResource* resource)
        : resource(resource)
    {
    }

    static void bus_watch(GstBus* bus, GstMessage* message, gpointer user_data)
    {
        QueryDurationData* data = reinterpret_cast<QueryDurationData*>(user_data);
        if (message->type == GST_MESSAGE_STATE_CHANGED
            || message->type == GST_MESSAGE_DURATION_CHANGED) {
            data->try_update_duration();
        }
    }

    void calculate_duration_async(const Gio::SlotAsyncReady& slot,
                                  const Glib::RefPtr<Gio::File>& file)
    {
        g_return_if_fail(file);
        QueryDurationData* data = new QueryDurationData(this);
        data->slot = slot;
        data->playbin = gst_element_factory_make("playbin", "playbin");
        g_object_set(data->playbin, "uri", file->get_uri().c_str(), NULL);
        gst_element_set_state(data->playbin, GST_STATE_PAUSED);
        data->bus = gst_element_get_bus(data->playbin);
        data->task = g_task_new(0, 0, QueryDurationData::async_ready_proxy, data);
        g_task_set_task_data(data->task, data, delete_data);

        if (!data->try_update_duration()) {
            gst_bus_add_signal_watch(data->bus);
            g_signal_connect(data->bus,
                             "message",
                             G_CALLBACK(Priv::bus_watch),
                             data);
        }
    }

    static void delete_data(gpointer d)
    {
        delete reinterpret_cast<QueryDurationData*>(d);
    }
};

Recording::Recording(ScRecordingResource* resource)
    : m_priv(new Priv(resource))
{
}

gint64 Recording::id() const
{
    return sc_recording_resource_get_id(resource());
}

Glib::RefPtr<Gio::File> Recording::file() const
{
    return Gio::File::create_for_path(sc_recording_resource_get_file(resource()));
}

const ScRecordingResource* Recording::resource() const
{
    return m_priv->resource.get();
}

ScRecordingResource* Recording::resource()
{
    return m_priv->resource.get();
}

std::tr1::shared_ptr<Recording> Recording::create(ScRecordingResource* resource)
{
    return std::tr1::shared_ptr<Recording>(new Recording(resource));
}

gint64 Recording::location_id() const
{
    return sc_recording_resource_get_location_id(resource());
}

int Recording::quality() const
{
    return sc_recording_resource_get_quality(resource());
}

Glib::DateTime Recording::date() const
{
    return Glib::wrap(sc_recording_resource_get_date(resource()));
}

Glib::ustring Recording::recordist() const
{
    return make_string(sc_recording_resource_get_recordist(resource()));
}

float Recording::elevation() const
{
    return sc_recording_resource_get_elevation(resource());
}

Glib::ustring Recording::remarks() const
{
    return make_string(sc_recording_resource_get_remarks(resource()));
}

float Recording::duration() const
{
    return sc_recording_resource_get_duration(resource());
}

void Recording::calculate_duration_async(const Gio::SlotAsyncReady& slot)
{
    m_priv->calculate_duration_async(slot, file());
}

bool Recording::calculate_duration_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
                                          float& duration)
{
    GTask* task = G_TASK(result->gobj());
    GError* error = 0;
    Priv::QueryDurationData* data = reinterpret_cast<Priv::QueryDurationData*>(g_task_get_task_data(task));

    bool status = g_task_propagate_boolean(task, &error);
    if (error)
        throw Glib::Error(error);

    duration = data->duration;
    return status;
}
}
