/*
 * simple-audio-player.cc
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

#include "simple-audio-player.h"

namespace SC {

struct SimpleAudioPlayer::Priv {
    Glib::RefPtr<Gio::File> file;
    GstElement* pipeline;
    GstElement* playbin;

    Priv(const Glib::RefPtr<Gio::File>& f)
        : playbin(gst_element_factory_make("playbin", "playbin"))
    {
        set_file(f);
    }

    ~Priv()
    {
        gst_element_set_state(playbin, GST_STATE_NULL);
        gst_object_unref(playbin);
    }

    void set_file(const Glib::RefPtr<Gio::File>& f)
    {
        file = f;
        gst_element_set_state(playbin, GST_STATE_NULL);
        g_object_set(playbin, "uri", f->get_uri().c_str(), NULL);
        gst_element_set_state(playbin, GST_STATE_PAUSED);
    }
};

SimpleAudioPlayer::SimpleAudioPlayer(const Glib::RefPtr<Gio::File>& file)
    : m_priv(new Priv(file))
{
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_priv->playbin));
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(SimpleAudioPlayer::on_pipeline_message_proxy), this);
}

void SimpleAudioPlayer::on_pipeline_message_proxy(GstBus* bus, GstMessage* message, gpointer user_data)
{
    SimpleAudioPlayer* self = reinterpret_cast<SimpleAudioPlayer*>(user_data);
    self->on_pipeline_message(bus, message);
}

void SimpleAudioPlayer::on_pipeline_message(GstBus* bus, GstMessage* message)
{
    switch (message->type) {
    case GST_MESSAGE_STATE_CHANGED:
        pipeline_state_changed();
        break;
    }
}

void SimpleAudioPlayer::pipeline_state_changed()
{
    GstState state = GST_STATE_VOID_PENDING;
    gst_element_get_state(m_priv->playbin, &state, 0, 100 * GST_MSECOND);
    switch (state) {
    case GST_STATE_PLAYING:
        set_sensitive(true);
        set_image_from_icon_name("media-playback-pause");
        break;
    case GST_STATE_PAUSED:
        set_sensitive(true);
        set_image_from_icon_name("media-playback-start");
        break;
    default:
        set_sensitive(false);
    }
}

void SimpleAudioPlayer::on_clicked()
{
    GstState state = GST_STATE_VOID_PENDING;
    gst_element_get_state(m_priv->playbin, &state, 0, 100 * GST_MSECOND);
    if (state == GST_STATE_PLAYING)
        gst_element_set_state(m_priv->playbin, GST_STATE_PAUSED);
    else
        gst_element_set_state(m_priv->playbin, GST_STATE_PLAYING);
}
}
