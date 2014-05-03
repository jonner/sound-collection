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
        QueryDurationSlot slot;
        GstElement* playbin;
        GstBus* bus;

        ~QueryDurationData()
        {
            if (playbin)
                gst_object_unref(playbin);
            if (bus)
                gst_object_unref(bus);
        }

        bool try_update_duration()
        {
            gint64 duration = GST_CLOCK_TIME_NONE;
            GstState state = GST_STATE_NULL;
            if (gst_element_get_state(playbin, &state, NULL, GST_CLOCK_TIME_NONE) == GST_STATE_CHANGE_SUCCESS
                && state == GST_STATE_PAUSED) {
                if (gst_element_query_duration(playbin, GST_FORMAT_TIME, &duration)) {
                    float seconds = static_cast<float>(GST_TIME_AS_MSECONDS(duration)) / 1000.0;
                    Glib::signal_idle().connect_once(sigc::bind(slot, seconds));
                    gst_bus_remove_signal_watch(bus);
                    delete this;
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

    static gboolean bus_watch(GstBus* bus, GstMessage* message, gpointer user_data)
    {
        QueryDurationData* data = reinterpret_cast<QueryDurationData*>(user_data);
        if (message->type == GST_MESSAGE_STATE_CHANGED
            || message->type == GST_MESSAGE_DURATION_CHANGED) {
            if (data->try_update_duration()) {
                return false;
            }
        }
        return true;
    }

    void calculate_duration_async(const QueryDurationSlot& slot,
                                  const Glib::RefPtr<Gio::File>& file)
    {
        QueryDurationData* data = new QueryDurationData();
        data->slot = slot;
        data->priv = this;
        data->playbin = gst_element_factory_make("playbin", "playbin");
        g_object_set(data->playbin, "uri", file->get_uri().c_str(), NULL);
        gst_element_set_state(data->playbin, GST_STATE_PAUSED);
        data->bus = gst_element_get_bus(data->playbin);
        gst_bus_add_signal_watch(data->bus);
        g_signal_connect(data->bus,
                         "message",
                         G_CALLBACK(Priv::bus_watch),
                         this);
        data->try_update_duration();
    }
};

Recording::Recording(ScRecordingResource* resource)
    : m_priv(new Priv(resource))
{
}

Recording::Recording(const Glib::RefPtr<Gio::File>& file)
    : m_priv(new Priv(SC_RECORDING_RESOURCE(g_object_new(SC_TYPE_RECORDING_RESOURCE,
                                                         "file",
                                                         file->get_path().c_str(),
                                                         NULL))))
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

void Recording::calculate_duration_async(const QueryDurationSlot& slot)
{
    m_priv->calculate_duration_async(slot, file());
}
}
