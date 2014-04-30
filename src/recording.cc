/*
 * recording.cc
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
#include "recording.h"

namespace SC {

struct Recording::Priv {
    WTF::GRefPtr<ScRecordingResource> resource;

    Priv(ScRecordingResource* resource)
        : resource(resource)
    {
    }
};

Recording::Recording(ScRecordingResource* resource)
    : m_priv(new Priv(resource))
{
}

gint64 Recording::id() const
{
    return sc_recording_resource_get_id(resource());
}

Glib::RefPtr<Gio::File> Recording::file() const
{
    return Gio::File::create_for_path(sc_recording_resource_get_file(resource()));
}

const ScRecordingResource* Recording::resource() const
{
    return m_priv->resource.get();
}

ScRecordingResource* Recording::resource()
{
    return m_priv->resource.get();
}

std::tr1::shared_ptr<Recording> Recording::create(ScRecordingResource* resource)
{
    return std::tr1::shared_ptr<Recording>(new Recording(resource));
}

Glib::ustring Recording::location() const
{
    return sc_recording_resource_get_location(resource());
}

Glib::ustring Recording::country() const
{
    return sc_recording_resource_get_country(resource());
}

float Recording::latitude() const
{
    return sc_recording_resource_get_latitude(resource());
}

float Recording::longitude() const
{
    return sc_recording_resource_get_longitude(resource());
}

int Recording::quality() const
{
    return sc_recording_resource_get_quality(resource());
}

Glib::DateTime Recording::date() const
{
    return Glib::wrap(sc_recording_resource_get_date(resource()));
}

Glib::ustring Recording::recordist() const
{
    return sc_recording_resource_get_recordist(resource());
}

float Recording::elevation() const
{
    return sc_recording_resource_get_elevation(resource());
}

Glib::ustring Recording::remarks() const
{
    return sc_recording_resource_get_remarks(resource());
}

float Recording::duration() const
{
    return sc_recording_resource_get_duration(resource());
}
}
