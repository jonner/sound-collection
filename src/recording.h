/*
 * recording.h
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

#ifndef _RECORDING_H
#define _RECORDING_H

#include <tr1/memory>
#include <giomm.h>
#include "recording-resource.h"

namespace SC {
class Recording {
public:
    static std::tr1::shared_ptr<Recording> create(ScRecordingResource* resource);

    gint64 id() const;
    Glib::RefPtr<Gio::File> file() const;
    gint64 location_id() const;
    int quality() const;
    Glib::DateTime date() const;
    Glib::ustring recordist() const;
    float elevation() const;
    Glib::ustring remarks() const;
    float duration() const;

    typedef sigc::slot<void, float> QueryDurationSlot;
    void calculate_duration_async(const QueryDurationSlot& slot);
    float calculate_duration_finish(const Glib::RefPtr<Gio::AsyncResult>& result);

    const ScRecordingResource* resource() const;
    ScRecordingResource* resource();

protected:
    Recording(ScRecordingResource* resource);

private:
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _RECORDING_H */

