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
    Priv(const std::tr1::shared_ptr<Recording>& rec,
         const std::tr1::shared_ptr<Repository>& repository)
        : recording(rec)
        , form(recording, repository)
    {
        titlebar.set_title(Glib::ustring::compose("Recording %1", recording->id()));
        titlebar.set_subtitle(recording->file()->get_path());
        titlebar.set_show_close_button(true);
        titlebar.show();
    }

    std::tr1::shared_ptr<Recording> recording;
    RecordingForm form;
    Gtk::HeaderBar titlebar;
};

RecordingWindow::RecordingWindow(const std::tr1::shared_ptr<Recording>& recording,
                                 const std::tr1::shared_ptr<Repository>& repository)
    : m_priv(new Priv(recording, repository))
{
    set_titlebar(m_priv->titlebar);
    set_default_size(400, 400);
    add_form(m_priv->form);
}

static bool delete_recording_window(GdkEventAny* event G_GNUC_UNUSED,
                                    RecordingWindow* window)
{
    g_debug("%s", G_STRFUNC);
    delete window;
    return false;
}

void RecordingWindow::display(const std::tr1::shared_ptr<Recording>& recording,
                              const std::tr1::shared_ptr<Repository>& repository)
{
    RecordingWindow* win = new RecordingWindow(recording, repository);
    win->signal_delete_event().connect(sigc::bind(sigc::ptr_fun(delete_recording_window), win));
    win->show();
}
}
