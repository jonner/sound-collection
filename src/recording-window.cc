/*
 * recording-window.cc
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

#include "GRefPtr.h"
#include "recording-window.h"
#include "recording-form.h"

namespace SC {

struct RecordingWindow::Priv {
    Priv(const std::tr1::shared_ptr<Recording>& rec)
        : recording(rec)
        , layout(Gtk::ORIENTATION_VERTICAL)
        , form(recording)
    {
        titlebar.set_title(Glib::ustring::compose("Recording %1", recording->id()));
        titlebar.set_subtitle(recording->file()->get_path());
        titlebar.set_show_close_button(true);
        titlebar.show();
        form.property_margin() = 10;
        layout.pack_start(form);
        layout.show_all();
    }

    std::tr1::shared_ptr<Recording> recording;
    Gtk::Box layout;
    RecordingForm form;
    Gtk::HeaderBar titlebar;
};

RecordingWindow::RecordingWindow(const std::tr1::shared_ptr<Recording>& recording)
    : m_priv(new Priv(recording))
{
    get_content_area()->pack_start(m_priv->layout, true, true);
    set_titlebar(m_priv->titlebar);
    set_default_size(400, 400);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Save", Gtk::RESPONSE_APPLY);
    signal_response().connect(sigc::mem_fun(this, &RecordingWindow::on_response));
}

static bool delete_recording_window(GdkEventAny* event G_GNUC_UNUSED,
                                    RecordingWindow* window)
{
    g_debug("%s", G_STRFUNC);
    delete window;
    return false;
}

void RecordingWindow::display(const std::tr1::shared_ptr<Recording>& recording)
{
    RecordingWindow* win = new RecordingWindow(recording);
    win->signal_delete_event().connect(sigc::bind(sigc::ptr_fun(delete_recording_window), win));
    win->show();
}

static void resource_saved(GObject* source, GAsyncResult* result, gpointer user_data)
{
    GError* error = 0;
    GomResource* resource = reinterpret_cast<GomResource*>(source);
    if (!gom_resource_save_finish(resource, result, &error)) {
        g_warning("Failed to save resource: %s", error->message);
        g_clear_error(&error);
    }
}

void RecordingWindow::on_response(int response_id)
{
    if (response_id == Gtk::RESPONSE_APPLY) {
        gom_resource_save_async(GOM_RESOURCE(m_priv->recording->resource()), resource_saved, this);
    }
    hide();
}
}
