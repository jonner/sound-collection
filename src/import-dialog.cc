/*
 * import-dialog.cc
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

#include "import-dialog.h"
#include "header-label.h"

namespace SC {
struct ImportDialog::Priv {
    Gtk::HeaderBar header;

    Priv()
    {
        header.set_title(Glib::get_prgname());
        header.set_subtitle("Import Recordings");
    }
};

ImportDialog::ImportDialog(Gtk::Window& parent)
    : Gtk::FileChooserDialog(parent, "Import Recordings")
    , m_priv(new Priv())
{
    set_titlebar(m_priv->header);
    set_select_multiple(true);
    Glib::RefPtr<Gtk::FileFilter> audio_filter = Gtk::FileFilter::create();
    audio_filter->add_mime_type("audio/*");
    audio_filter->set_name("Audio Files");
    set_filter(audio_filter);
}
}
