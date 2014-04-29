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

    Priv(const std::tr1::shared_ptr<Recording>& rec)
        : recording(rec)
        , id_label("ID")
        , file_label("File")
        , preview_label("Preview")
        , preview_player(rec->file())
    {
        id_label.show();
        id_value_label.set_text(Glib::ustring::format(recording->id()));
        id_value_label.show();
        file_label.show();
        file_entry.set_text(recording->file()->get_path());
        file_entry.show();
        preview_label.show();
        preview_player.show();
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
    attach(m_priv->file_entry, 1, 1, 1, 1);
    attach(m_priv->preview_label, 0, 2, 1, 1);
    attach(m_priv->preview_player, 1, 2, 1, 1);
}
}
