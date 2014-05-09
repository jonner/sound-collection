/*
 * location-form.cc
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

#include "location-form.h"

namespace SC {

struct LocationForm::Priv {
    std::tr1::shared_ptr<Location> location;
    Gtk::Label id_label;
    Gtk::Label id_value_label;
    Gtk::Label name_label;
    Gtk::Entry name_entry;
    Gtk::Label country_label;
    Gtk::Entry country_entry;
    Gtk::Label latitude_label;
    Gtk::Entry latitude_entry;
    Gtk::Label longitude_label;
    Gtk::Entry longitude_entry;

    Priv(const std::tr1::shared_ptr<Location>& loc)
        : location(loc)
        , id_label("ID", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , id_value_label("", Gtk::ALIGN_START, Gtk::ALIGN_CENTER)
        , name_label("Location", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , country_label("Country", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , latitude_label("Latitude", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)
        , longitude_label("Longitude", Gtk::ALIGN_END, Gtk::ALIGN_CENTER)

    {
        Pango::Attribute bold = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
        Pango::AttrList attrs;
        attrs.insert(bold);
        id_label.show();
        id_label.set_attributes(attrs);
        id_value_label.set_text(Glib::ustring::format(location->id()));
        id_value_label.show();
        id_value_label.set_hexpand(true);
        name_label.show();
        name_label.set_attributes(attrs);
        name_entry.show();
        name_entry.set_text(location->name());
        country_label.show();
        country_label.set_attributes(attrs);
        country_entry.show();
        country_entry.set_text(location->country());
        latitude_label.show();
        latitude_label.set_attributes(attrs);
        latitude_entry.show();
        latitude_entry.set_input_purpose(Gtk::INPUT_PURPOSE_NUMBER);
        latitude_entry.set_text(Glib::ustring::format(location->latitude()));
        longitude_label.show();
        longitude_label.set_attributes(attrs);
        longitude_entry.show();
        longitude_entry.set_input_purpose(Gtk::INPUT_PURPOSE_NUMBER);
        longitude_entry.set_text(Glib::ustring::format(location->longitude()));
    }
};

LocationForm::LocationForm(const std::tr1::shared_ptr<Location>& location)
    : m_priv(new Priv(location))
{
    set_column_homogeneous(false);
    set_row_homogeneous(true);
    set_row_spacing(5);
    set_column_spacing(10);
    attach(m_priv->id_label, 0, 0, 1, 1);
    attach(m_priv->id_value_label, 1, 0, 1, 1);
    attach(m_priv->name_label, 0, 2, 1, 1);
    attach(m_priv->name_entry, 1, 2, 3, 1);
    attach(m_priv->country_label, 0, 3, 1, 1);
    attach(m_priv->country_entry, 1, 3, 3, 1);
    attach(m_priv->latitude_label, 0, 4, 1, 1);
    attach(m_priv->latitude_entry, 1, 4, 3, 1);
    attach(m_priv->longitude_label, 0, 5, 1, 1);
    attach(m_priv->longitude_entry, 1, 5, 3, 1);
}

GomResource* LocationForm::resource()
{
    return GOM_RESOURCE(m_priv->location->resource());
}

}
