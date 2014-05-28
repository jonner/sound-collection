/*
 * util.cc
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

#include <iomanip>

#include "util.h"

namespace SC {

Glib::ustring format_duration(double seconds)
{
    int h = 0, m = 0, s = 0;

    h = seconds / (60 * 60);
    m = (seconds - (h * 60 * 60)) / 60;
    s = seconds - (h * 60 * 60) - (m * 60);
    Glib::ustring formatted;

    if (h)
        formatted += Glib::ustring::compose("%1",
                                            Glib::ustring::format(std::setfill(L'0'), std::setw(2), h));
    formatted += Glib::ustring::compose("%1:%2",
                                        Glib::ustring::format(std::setfill(L'0'), std::setw(h ? 2 : 1), m),
                                        Glib::ustring::format(std::setfill(L'0'), std::setw(2), s));

    return formatted;
}
}
