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
    return sc_recording_resource_get_id(m_priv->resource.get());
}

Glib::RefPtr<Gio::File> Recording::file() const
{
    return Gio::File::create_for_path(sc_recording_resource_get_file(m_priv->resource.get()));
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
}
