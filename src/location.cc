/*
 * location.cc
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
#include "location.h"
#include "location-resource.h"

namespace SC {

static Glib::ustring make_string(const char* cstr)
{
    if (cstr)
        return Glib::ustring(cstr);
    return "";
}

struct Location::Priv {
    WTF::GRefPtr<ScLocationResource> resource;

    Priv(ScLocationResource* res)
        : resource(res)
    {
    }
};

Location::Location(ScLocationResource* resource)
    : m_priv(new Priv(resource))
{
}

const ScLocationResource* Location::resource() const
{
    return m_priv->resource.get();
}

ScLocationResource* Location::resource()
{
    return m_priv->resource.get();
}

gint64 Location::id() const
{
    return sc_location_resource_get_id(resource());
}

Glib::ustring Location::name() const
{
    return make_string(sc_location_resource_get_name(resource()));
}

Glib::ustring Location::country() const
{
    return make_string(sc_location_resource_get_country(resource()));
}

float Location::latitude() const
{
    return sc_location_resource_get_latitude(resource());
}

float Location::longitude() const
{
    return sc_location_resource_get_longitude(resource());
}

std::tr1::shared_ptr<Location> Location::create(ScLocationResource* resource)
{
    return std::tr1::shared_ptr<Location>(new Location(resource));
}
}
