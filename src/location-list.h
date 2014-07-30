/*
 * location-list.h
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

#ifndef _LOCATION_LIST_H
#define _LOCATION_LIST_H

#include <tr1/memory>
#include <gtkmm.h>

#include "location.h"
#include "repository.h"

namespace SC {

class LocationList : public Gtk::Box {
public:
    LocationList(const std::tr1::shared_ptr<Repository>& repository);

private:
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _LOCATION_LIST_H */

