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
#include "task.h"

namespace SC {

static Glib::ustring make_string(const char* cstr)
{
    if (cstr)
        return Glib::ustring(cstr);
    return "";
}

struct Recording::Priv {
    WTF::GRefPtr<ScRecordingResource> resource;

    class CalculateDurationTask : public Task {
    public:
        Recording::Priv* priv;
        GstElement* playbin;
        GstBus* bus;
        float duration;

        CalculateDurationTask(const Gio::SlotAsyncReady& slot, Recording::Priv* priv)
            : Task(slot)
            , priv(priv)
            , playbin(0)
            , bus(0)
            , duration(0.0)
        {
        }

        ~CalculateDurationTask()
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
                    g_task_return_boolean(task(), true);
                    return true;
                }
            }
            return false;
        }
    };

    Priv(ScRecordingResource* resource)
        : resource(resource)
    {
    }

    static void bus_watch(GstBus* bus, GstMessage* message, gpointer user_data)
    {
        CalculateDurationTask* task = reinterpret_cast<CalculateDurationTask*>(user_data);
        if (message->type == GST_MESSAGE_STATE_CHANGED
            || message->type == GST_MESSAGE_DURATION_CHANGED) {
            task->try_update_duration();
        }
    }

    void calculate_duration_async(const Gio::SlotAsyncReady& slot,
                                  const Glib::RefPtr<Gio::File>& file)
    {
        g_return_if_fail(file);
        CalculateDurationTask* task = new CalculateDurationTask(slot, this);
        task->playbin = gst_element_factory_make("playbin", "playbin");
        g_object_set(task->playbin, "uri", file->get_uri().c_str(), NULL);
        gst_element_set_state(task->playbin, GST_STATE_PAUSED);
        task->bus = gst_element_get_bus(task->playbin);

        if (!task->try_update_duration()) {
            gst_bus_add_signal_watch(task->bus);
            g_signal_connect(task->bus,
                             "message",
                             G_CALLBACK(Priv::bus_watch),
                             task);
        }
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
    return Glib::wrap(sc_recording_resource_get_date(resource()), true);
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
    GTask* gtask = G_TASK(result->gobj());
    GError* error = 0;
    Priv::CalculateDurationTask* task = reinterpret_cast<Priv::CalculateDurationTask*>(g_task_get_task_data(gtask));

    bool status = g_task_propagate_boolean(gtask, &error);
    if (error)
        throw Glib::Error(error);

    duration = task->duration;
    return status;
}
}
