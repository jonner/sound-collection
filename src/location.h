/*
 * location.h
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

#ifndef _LOCATION_H
#define _LOCATION_H

#include <glibmm.h>
#include <tr1/memory>
#include "location-resource.h"

namespace SC {
class Location {
public:
    static std::tr1::shared_ptr<Location> create(ScLocationResource* resource);

    gint64 id() const;
    Glib::ustring name() const;
    Glib::ustring country() const;
    float latitude() const;
    float longitude() const;

protected:
    Location(ScLocationResource* resource);
    const ScLocationResource* resource() const;
    ScLocationResource* resource();

private:
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _LOCATION_H */

