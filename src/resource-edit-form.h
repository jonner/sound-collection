/*
 * resource-edit-form.h
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

#ifndef _RESOURCE_EDIT_FORM_H
#define _RESOURCE_EDIT_FORM_H

#include <gom/gom.h>
#include <gtkmm.h>

namespace SC {

class ResourceEditForm : public Gtk::Grid {
public:
    virtual GomResource* resource() = 0;
};
}

#endif /* _RESOURCE_EDIT_FORM_H */

