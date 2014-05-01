/*
 * recording-form.cc
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

#include "recording-form.h"
#include "simple-audio-player.h"

namespace SC {

struct RecordingForm::Priv {
    std::tr1::shared_ptr<Recording> recording;
    Gtk::Label id_label;
    Gtk::Label id_value_label;
    Gtk::Label file_label;
    Gtk::Entry file_entry;
    Gtk::Label preview_label;
    SimpleAudioPlayer preview_player;
    Gtk::Label duration_label;
    Gtk::Label duration_value_label;
    Gtk::Button duration_update_button;
    Gtk::Label remarks_label;
    Gtk::ScrolledWindow remarks_scroll;
    Gtk::TextView remarks_entry;
    GstElement* playbin;
    GstBus* bus;

    Priv(const std::tr1::shared_ptr<Recording>& rec)
        : recording(rec)
        , id_label("ID", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , id_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , file_label("File", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , preview_label("Preview", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , preview_player(rec->file())
        , duration_label("Duration", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , duration_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , remarks_label("Remarks", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , playbin(0)
        , bus(0)
    {
        Pango::Attribute bold = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
        Pango::AttrList attrs;
        attrs.insert(bold);
        id_label.show();
        id_label.set_attributes(attrs);
        id_value_label.set_text(Glib::ustring::format(recording->id()));
        id_value_label.show();
        id_value_label.set_hexpand(true);
        file_label.show();
        file_label.set_attributes(attrs);
        file_entry.set_text(recording->file()->get_path());
        file_entry.show();
        file_entry.set_hexpand(true);
        preview_label.show();
        preview_label.set_attributes(attrs);
        preview_player.show();
        preview_player.set_hexpand(false);
        duration_label.show();
        duration_label.set_attributes(attrs);
        duration_value_label.set_text(Glib::ustring::format(recording->duration()));
        duration_value_label.show();
        duration_value_label.set_hexpand(false);
        duration_update_button.show();
        duration_update_button.set_image_from_icon_name("reload");
        duration_update_button.signal_clicked().connect(sigc::mem_fun(this, &Priv::on_update_duration_clicked));
        duration_update_button.set_hexpand(false);
        remarks_label.show();
        remarks_label.set_attributes(attrs);
        remarks_scroll.set_hexpand(true);
        remarks_scroll.add(remarks_entry);
        remarks_entry.get_buffer()->set_text(recording->remarks());
        remarks_entry.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
    }

    static gboolean bus_watch(GstBus* bus, GstMessage* message, gpointer user_data)
    {
        Priv* priv = reinterpret_cast<Priv*>(user_data);
        if (message->type == GST_MESSAGE_STATE_CHANGED
            || message->type == GST_MESSAGE_DURATION_CHANGED) {
            if (priv->try_update_duration()) {
                // remove source
                return false;
            }
        }
        return true;
    }

    bool try_update_duration()
    {
        gint64 duration = GST_CLOCK_TIME_NONE;
        GstState state = GST_STATE_NULL;
        if (gst_element_get_state(playbin, &state, NULL, GST_CLOCK_TIME_NONE) == GST_STATE_CHANGE_SUCCESS
            && state == GST_STATE_PAUSED) {
            if (gst_element_query_duration(playbin, GST_FORMAT_TIME, &duration)) {
                float seconds = static_cast<float>(GST_TIME_AS_MSECONDS(duration)) / 1000.0;
                duration_value_label.set_text(Glib::ustring::format(seconds));
                g_object_set(recording->resource(),
                             "duration",
                             seconds,
                             NULL);
                gom_resource_save_sync(GOM_RESOURCE(recording->resource()), 0);
                return true;
            }
        }
        return false;
    }

    void on_update_duration_clicked()
    {
        if (!playbin) {
            playbin = gst_element_factory_make("playbin", "playbin");
            g_object_set(playbin, "uri", recording->file()->get_uri().c_str(), NULL);
            gst_element_set_state(playbin, GST_STATE_PAUSED);
            bus = gst_element_get_bus(playbin);
        }
        if (!try_update_duration())
            gst_bus_add_watch(bus, &Priv::bus_watch, this);
    }

    ~Priv()
    {
        if (playbin)
            gst_object_unref(playbin);
        if (bus)
            gst_object_unref(bus);
    }
};

RecordingForm::RecordingForm(const std::tr1::shared_ptr<Recording>& recording)
    : m_priv(new Priv(recording))
{
    set_column_homogeneous(false);
    set_row_homogeneous(true);
    set_row_spacing(5);
    set_column_spacing(10);
    attach(m_priv->id_label, 0, 0, 1, 1);
    attach(m_priv->id_value_label, 1, 0, 1, 1);
    attach(m_priv->file_label, 0, 1, 1, 1);
    attach(m_priv->file_entry, 1, 1, 3, 1);
    attach(m_priv->preview_label, 0, 2, 1, 1);
    attach(m_priv->preview_player, 1, 2, 1, 1);
    attach(m_priv->duration_label, 0, 3, 1, 1);
    attach(m_priv->duration_value_label, 1, 3, 1, 1);
    attach(m_priv->duration_update_button, 2, 3, 1, 1);
    attach(m_priv->remarks_label, 0, 4, 1, 1);
    attach(m_priv->remarks_scroll, 0, 5, 4, 4);
}
}
