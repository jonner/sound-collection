/*
 * welcome-screen.cc
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

#include "location-list.h"
#include "recording-list.h"
#include "welcome-screen.h"

namespace SC {

struct WelcomeScreen::Priv {
    std::tr1::shared_ptr<Repository> repository;
    Gtk::Stack stack;
    Gtk::StackSwitcher switcher;
    RecordingList recordings;
    LocationList locations;

    Priv(const std::tr1::shared_ptr<Repository>& repository)
        : repository(repository)
        , recordings(repository)
        , locations(repository)
    {
        switcher.set_stack(stack);

        locations.show();
        recordings.show();

        stack.add(recordings, "recordings", "Recordings");
        stack.add(locations, "locations", "Locations");

        stack.set_visible_child("recordings");
    }
};

WelcomeScreen::WelcomeScreen(const std::tr1::shared_ptr<Repository>& repository)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    , m_priv(new Priv(repository))
{
    m_priv->switcher.show();
    m_priv->stack.show();
    Gtk::Alignment* align = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER));
    align->show();
    align->add(m_priv->switcher);
    pack_start(*align, false, true, 6);
    pack_start(m_priv->stack, true, true);
}
}
