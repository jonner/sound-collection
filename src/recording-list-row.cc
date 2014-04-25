/*
 * recording-list-row.cc
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
#include "recording-list-row.h"

namespace SC {
struct RecordingListRow::Priv {
    Priv(ScRecordingResource* rec)
        : recording(rec)
    {
        layout.set_column_spacing(10);
        layout.set_row_spacing(5);
        layout.attach(id_label, 0, 0, 1, 1);
        layout.attach(file_label, 1, 0, 1, 1);
        layout.attach(recordist_label, 2, 0, 1, 1);
        layout.attach(location_label, 3, 0, 1, 1);
        layout.attach(date_label, 4, 0, 1, 1);
        layout.show_all();

        guint64 id = -1;
        GDateTime* gdate = 0;
        char* recordist = 0;
        char* country = 0;
        char* location = 0;
        char* file = 0;
        g_object_get(recording.get(),
                     "id",
                     &id,
                     "date",
                     &gdate,
                     "recordist",
                     &recordist,
                     "location",
                     &location,
                     "country",
                     &country,
                     "file",
                     &file,
                     NULL);
        g_debug("%s: got recording %i", G_STRFUNC, id);

        Glib::DateTime date = Glib::wrap(gdate);
        id_label.set_text(Glib::ustring::format(id));
        recordist_label.set_text(recordist);
        date_label.set_text(date.format("%F %R"));
        location_label.set_text(Glib::ustring::compose("%1, %2", location, country));
        file_label.set_text(file);

        g_free(recordist);
        g_free(location);
        g_free(country);
        g_free(file);
    }

    WTF::GRefPtr<ScRecordingResource> recording;
    Gtk::Grid layout;
    Gtk::Label id_label;
    Gtk::Label date_label;
    Gtk::Label recordist_label;
    Gtk::Label location_label;
    Gtk::Label file_label;
};

RecordingListRow::RecordingListRow(ScRecordingResource* recording)
    : m_priv(new Priv(recording))
{

    add(m_priv->layout);
}
}
