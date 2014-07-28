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

#include "header-label.h"
#include "location-tree-model.h"
#include "recording-form.h"
#include "simple-audio-player.h"
#include "util.h"

namespace SC {

struct RecordingForm::Priv {
    std::tr1::shared_ptr<Recording> recording;
    std::tr1::shared_ptr<Repository> repository;
    HeaderLabel id_label;
    Gtk::Label id_value_label;
    HeaderLabel file_label;
    Gtk::Label file_value_label;
    Gtk::Button file_open_button;
    HeaderLabel preview_label;
    SimpleAudioPlayer preview_player;
    HeaderLabel duration_label;
    Gtk::Label duration_value_label;
    Gtk::Button duration_update_button;
    HeaderLabel quality_label;
    Gtk::SpinButton quality_entry;
    HeaderLabel recordist_label;
    Gtk::Entry recordist_entry;
    HeaderLabel elevation_label;
    Gtk::SpinButton elevation_entry;
    HeaderLabel remarks_label;
    Gtk::ScrolledWindow remarks_scroll;
    Gtk::TextView remarks_entry;
    Gtk::Label date_label;
    Gtk::Label date_value_label;
    HeaderLabel location_label;
    Gtk::ComboBox location_selector;
    Glib::RefPtr<LocationTreeModel> location_model;

    Priv(const std::tr1::shared_ptr<Recording>& rec,
         const std::tr1::shared_ptr<Repository>& repository)
        : recording(rec)
        , repository(repository)
        , id_label("ID", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , id_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , file_label("File", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , file_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , preview_label("Preview", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , preview_player(rec->file())
        , duration_label("Duration", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , duration_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , recordist_label("Recordist", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , quality_label("Quality (0-5)", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , elevation_label("Elevation (m)", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , remarks_label("Remarks", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , date_label("Date", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , date_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , location_label("Location", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , location_model(LocationTreeModel::create())
    {
        id_label.show();
        id_value_label.set_text(Glib::ustring::format(recording->id()));
        id_value_label.show();
        id_value_label.set_hexpand(true);
        file_label.show();
        file_value_label.set_text(recording->file()->get_path());
        file_value_label.show();
        file_value_label.set_hexpand(true);
        file_value_label.set_ellipsize(Pango::ELLIPSIZE_START);
        file_open_button.set_image_from_icon_name("applications-multimedia");
        file_open_button.show();
        file_open_button.signal_clicked().connect(sigc::mem_fun(this, &Priv::on_file_open_clicked));
        preview_label.show();
        preview_player.show();
        preview_player.set_hexpand(false);
        duration_label.show();
        duration_value_label.set_text(format_duration(recording->duration()));
        duration_value_label.show();
        duration_value_label.set_hexpand(false);
        duration_update_button.show();
        duration_update_button.set_image_from_icon_name("reload");
        duration_update_button.signal_clicked().connect(sigc::mem_fun(this, &Priv::on_update_duration_clicked));
        duration_update_button.set_hexpand(false);
        recordist_label.show();
        recordist_entry.set_text(recording->recordist());
        recordist_entry.show();
        recordist_entry.set_hexpand(true);
        quality_label.show();
        quality_entry.show();
        quality_entry.set_hexpand(true);
        quality_entry.set_range(0.0, 5.0);
        quality_entry.set_increments(1.0, 1.0);
        quality_entry.set_digits(0);
        quality_entry.set_numeric(true);
        quality_entry.set_value(recording->quality());
        elevation_label.show();
        elevation_entry.show();
        elevation_entry.set_hexpand(true);
        elevation_entry.set_range(-G_MAXFLOAT, G_MAXFLOAT);
        elevation_entry.set_increments(20, 200);
        elevation_entry.set_digits(1);
        elevation_entry.set_numeric(true);
        elevation_entry.set_value(recording->elevation());
        location_label.show();
        location_selector.show();
        location_selector.set_hexpand(true);
        remarks_label.show();
        remarks_scroll.set_hexpand(true);
        remarks_scroll.add(remarks_entry);
        remarks_scroll.show();
        remarks_entry.show();
        remarks_entry.get_buffer()->set_text(recording->remarks());
        remarks_entry.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
        date_label.show();
        Glib::DateTime d = recording->date();
        date_value_label.show();
        if (d.to_unix() == 0) {
            date_value_label.set_markup("<i>unspecified</i>");
        } else {
            date_value_label.set_text(d.format("%c"));
        }

        repository->get_locations_async(sigc::mem_fun(this, &Priv::got_locations));

        // handlers for applying changes to the form
        remarks_entry.get_buffer()->signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        recordist_entry.signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        quality_entry.signal_value_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        elevation_entry.signal_value_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
        location_selector.signal_changed().connect(sigc::mem_fun(this, &Priv::on_property_changed));
    }

    void set_active_location(const std::tr1::shared_ptr<Location>& location)
    {
        g_debug("%s: setting active location to %u", G_STRFUNC, location->id());
        g_debug("Checking %u locations", location_model->children().size());
        for (Gtk::TreeModel::iterator it = location_model->children().begin();
             it != location_model->children().end();
             ++it) {
            guint64 id = (*it)[location_model->columns().id];
            g_debug("checking location %u", id);
            if (id == location->id()) {
                location_selector.set_active(it);
                break;
            }
        }
    }

    void got_location(const std::tr1::shared_ptr<Location>& location)
    {
        if (location) {
            g_debug("Got location %s", location->name().c_str());
            set_active_location(location);
        } else
            g_debug("Unable to get location");
    }

    void got_locations(const Glib::RefPtr<Gio::AsyncResult>& result)
    {
        try
        {
            GomResourceGroup* locations = repository->get_locations_finish(result);
            gom_resource_group_fetch_async(locations, 0, gom_resource_group_get_count(locations), &Priv::on_locations_fetched_proxy, this);
        }
        catch (const Glib::Error& e)
        {
            g_warning(e.what().c_str());
            return;
        }
    }

    static void on_locations_fetched_proxy(GObject* source,
                                           GAsyncResult* result,
                                           gpointer user_data)
    {
        GError* error = 0;
        Priv* self = reinterpret_cast<Priv*>(user_data);
        GomResourceGroup* locations = GOM_RESOURCE_GROUP(source);
        if (!gom_resource_group_fetch_finish(locations,
                                             result,
                                             &error)) {
            g_error("%s", error->message);
            g_clear_error(&error);
            return;
        }
        self->on_locations_fetched(locations);
    }

    void on_locations_fetched(GomResourceGroup* locations)
    {
        location_model->set_resource_group(locations);
        location_selector.set_model(location_model);
        location_selector.pack_start(location_model->columns().name);

        recording->get_location_async(sigc::mem_fun(this, &Priv::got_location));
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
        gint64 location_id = 0;
        Gtk::TreeModel::iterator active_iter = location_selector.get_active();
        if (active_iter)
            location_id = (*active_iter)[location_model->columns().id];
        g_object_set(recording->resource(),
                     "remarks",
                     remarks_entry.get_buffer()->get_text().c_str(),
                     "recordist",
                     recordist_entry.get_text().c_str(),
                     "quality",
                     static_cast<int>(quality_entry.get_value_as_int()),
                     "elevation",
                     static_cast<float>(elevation_entry.get_value()),
                     "location-id",
                     static_cast<gint64>(location_id),
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

RecordingForm::RecordingForm(const std::tr1::shared_ptr<Recording>& recording,
                             const std::tr1::shared_ptr<Repository>& repository)
    : m_priv(new Priv(recording, repository))
{
    set_column_homogeneous(false);
    set_row_homogeneous(true);
    set_row_spacing(5);
    set_column_spacing(10);
    attach(m_priv->id_label, 0, 0, 1, 1);
    attach_next_to(m_priv->id_value_label, m_priv->id_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->file_label, m_priv->id_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->file_value_label, m_priv->file_label, Gtk::POS_RIGHT, 1, 1);
    attach_next_to(m_priv->file_open_button, m_priv->file_value_label, Gtk::POS_RIGHT, 1, 1);
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
    attach_next_to(m_priv->location_label, m_priv->elevation_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->location_selector, m_priv->location_label, Gtk::POS_RIGHT, 3, 1);
    attach_next_to(m_priv->remarks_label, m_priv->location_label, Gtk::POS_BOTTOM, 1, 1);
    attach_next_to(m_priv->remarks_scroll, m_priv->remarks_label, Gtk::POS_BOTTOM, 4, 4);
}

GomResource* RecordingForm::resource()
{
    return GOM_RESOURCE(m_priv->recording->resource());
}
}
