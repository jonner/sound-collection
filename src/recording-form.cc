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
    Gtk::Button file_open_button;
    Gtk::Label preview_label;
    SimpleAudioPlayer preview_player;
    Gtk::Label duration_label;
    Gtk::Label duration_value_label;
    Gtk::Button duration_update_button;
    Gtk::Label quality_label;
    Gtk::SpinButton quality_entry;
    Gtk::Label recordist_label;
    Gtk::Entry recordist_entry;
    Gtk::Label elevation_label;
    Gtk::SpinButton elevation_entry;
    Gtk::Label remarks_label;
    Gtk::ScrolledWindow remarks_scroll;
    Gtk::TextView remarks_entry;
    Gtk::Label date_label;
    Gtk::Label date_value_label;

    Priv(const std::tr1::shared_ptr<Recording>& rec)
        : recording(rec)
        , id_label("ID", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , id_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , file_label("File", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , preview_label("Preview", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , preview_player(rec->file())
        , duration_label("Duration (s)", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , duration_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , recordist_label("Recordist", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , quality_label("Quality (0-5)", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , elevation_label("Elevation (m)", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , remarks_label("Remarks", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , date_label("Date", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , date_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
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
        file_open_button.set_image_from_icon_name("applications-multimedia");
        file_open_button.show();
        file_open_button.signal_clicked().connect(sigc::mem_fun(this, &Priv::on_file_open_clicked));
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
        recordist_label.show();
        recordist_label.set_attributes(attrs);
        recordist_entry.set_text(recording->recordist());
        recordist_entry.show();
        recordist_entry.set_hexpand(true);
        quality_label.show();
        quality_label.set_attributes(attrs);
        quality_entry.show();
        quality_entry.set_hexpand(true);
        quality_entry.set_range(0.0, 5.0);
        quality_entry.set_increments(1.0, 1.0);
        quality_entry.set_digits(0);
        quality_entry.set_numeric(true);
        quality_entry.set_value(recording->quality());
        elevation_label.show();
        elevation_label.set_attributes(attrs);
        elevation_entry.show();
        elevation_entry.set_hexpand(true);
        elevation_entry.set_range(-G_MAXFLOAT, G_MAXFLOAT);
        elevation_entry.set_increments(20, 200);
        elevation_entry.set_digits(1);
        elevation_entry.set_numeric(true);
        elevation_entry.set_value(recording->elevation());
        remarks_label.show();
        remarks_label.set_attributes(attrs);
        remarks_scroll.set_hexpand(true);
        remarks_scroll.add(remarks_entry);
        remarks_scroll.show();
        remarks_entry.show();
        remarks_entry.get_buffer()->set_text(recording->remarks());
        remarks_entry.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
        date_label.show();
        date_label.set_attributes(attrs);
        Glib::DateTime d = recording->date();
        date_value_label.show();

        // handlers for applying changes to the form
        file_entry.signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        remarks_entry.get_buffer()->signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        recordist_entry.signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        quality_entry.signal_value_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        elevation_entry.signal_value_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
    }

    void on_file_open_clicked()
    {
        Glib::RefPtr<Gio::File> file = recording->file();
        Glib::RefPtr<Gio::AppInfo> appinfo = file->query_default_handler();
        if (appinfo)
            appinfo->launch(file);
        else
            g_warning("Unable to launch file");
    }

    void on_property_changed()
    {
        g_debug("Updating resource: quality=%i, elevation=%g", quality_entry.get_value_as_int(), elevation_entry.get_value());
        g_object_set(recording->resource(),
                     "file",
                     file_entry.get_text().c_str(),
                     "remarks",
                     remarks_entry.get_buffer()->get_text().c_str(),
                     "recordist",
                     recordist_entry.get_text().c_str(),
                     "quality",
                     static_cast<int>(quality_entry.get_value_as_int()),
                     "elevation",
                     static_cast<float>(elevation_entry.get_value()),
                     NULL);
    }

    void on_query_duration_finished(const Glib::RefPtr<Gio::AsyncResult>& result)
    {
        float seconds;
        try
        {
            recording->calculate_duration_finish(result, seconds);
        }
        catch (const Glib::Error& error)
        {
            g_warning("Failed to calculate duration for recording: %s",
                      error.what().c_str());
            return;
        }
        duration_value_label.set_text(Glib::ustring::format(seconds));
        g_object_set(recording->resource(),
                     "duration",
                     seconds,
                     NULL);
    }

    void on_update_duration_clicked()
    {
        recording->calculate_duration_async(sigc::mem_fun(this, &Priv::on_query_duration_finished));
    }

    ~Priv()
    {
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
    attach_next_to(m_priv->id_value_label, m_priv->id_label, Gtk::POS_RIGHT, 1 ,1);
    attach_next_to(m_priv->file_label, m_priv->id_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->file_entry, m_priv->file_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->file_open_button, m_priv->file_entry, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->preview_label, m_priv->file_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->preview_player, m_priv->preview_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->duration_label, m_priv->preview_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->duration_value_label, m_priv->duration_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->duration_update_button, m_priv->duration_value_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->recordist_label, m_priv->duration_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->recordist_entry, m_priv->recordist_label, Gtk::POS_RIGHT, 3, 1);
    attach_next_to(m_priv->date_label, m_priv->recordist_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->date_value_label, m_priv->date_label, Gtk::POS_RIGHT, 3, 1);
    attach_next_to(m_priv->quality_label, m_priv->date_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->quality_entry, m_priv->quality_label, Gtk::POS_RIGHT, 3, 1);
    attach_next_to(m_priv->elevation_label, m_priv->quality_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->elevation_entry, m_priv->elevation_label, Gtk::POS_RIGHT, 3, 1);
    attach_next_to(m_priv->remarks_label, m_priv->elevation_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->remarks_scroll, m_priv->remarks_label, Gtk::POS_BOTTOM, 4, 4);
}

GomResource* RecordingForm::resource()
{
    return GOM_RESOURCE(m_priv->recording->resource());
}
}
