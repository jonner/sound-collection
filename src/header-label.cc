/*
 * header-label.cc
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
namespace SC {

HeaderLabel::HeaderLabel(const Glib::ustring& label, bool mnemonic)
    : Gtk::Label(label, mnemonic)
{
    init();
}

HeaderLabel::HeaderLabel()
    : Gtk::Label()
{
    init();
}

HeaderLabel::HeaderLabel(const Glib::ustring& label, Gtk::Align xalign, Gtk::Align yalign, bool mnemonic)
    : Gtk::Label(label, xalign, yalign, mnemonic)
{
    init();
}

void HeaderLabel::init()
{
    Pango::Attribute bold = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
    Pango::AttrList attrs;
    attrs.insert(bold);
    set_attributes(attrs);
}
}
