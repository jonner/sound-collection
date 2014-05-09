/*
 * header-label.h
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

#ifndef _HEADER_LABEL_H
#define _HEADER_LABEL_H

#include <gtkmm.h>

namespace SC {
class HeaderLabel : public Gtk::Label {
public:
    HeaderLabel();
    HeaderLabel(const Glib::ustring& label = Glib::ustring(), bool mnemonic = false);
    HeaderLabel(const Glib::ustring& label, Gtk::Align xalign, Gtk::Align yalign = Gtk::ALIGN_CENTER, bool mnemonic = false);

private:
    void init();
};
}

#endif /* _HEADER_LABEL_H */

