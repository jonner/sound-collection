/*
 * location-window.cc
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
#include "location-window.h"
#include "location-form.h"

namespace SC {

struct LocationWindow::Priv {
    Priv(const std::tr1::shared_ptr<Location>& rec)
        : location(rec)
        , form(location)
    {
        titlebar.set_title(Glib::ustring::compose("Location %1", location->id()));
        titlebar.set_subtitle(location->name());
        titlebar.set_show_close_button(true);
        titlebar.show();
    }

    std::tr1::shared_ptr<Location> location;
    LocationForm form;
    Gtk::HeaderBar titlebar;
};

LocationWindow::LocationWindow(const std::tr1::shared_ptr<Location>& location)
    : m_priv(new Priv(location))
{
    set_titlebar(m_priv->titlebar);
    set_default_size(400, 400);
    add_form(m_priv->form);
}

static bool delete_location_window(GdkEventAny* event G_GNUC_UNUSED,
                                    LocationWindow* window)
{
    g_debug("%s", G_STRFUNC);
    delete window;
    return false;
}

void LocationWindow::display(const std::tr1::shared_ptr<Location>& location)
{
    LocationWindow* win = new LocationWindow(location);
    win->signal_delete_event().connect(sigc::bind(sigc::ptr_fun(delete_location_window), win));
    win->show();
}
}
