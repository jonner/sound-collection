/*
 * main-window.cc
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
 * along with SoundCollectio. If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"
#include "GRefPtr.h"
#include "main-window.h"
#include "welcome-screen.h"

namespace SC {
struct MainWindow::Priv {
    std::tr1::shared_ptr<Repository> repository;
    Gtk::HeaderBar header;
    WelcomeScreen welcome_screen;

    Priv(const std::tr1::shared_ptr<Repository>& repository)
        : repository(repository)
        , welcome_screen(repository)
    {
        header.set_show_close_button(true);
        header.show();
        welcome_screen.show();
    }

    void application_changed(MainWindow* window)
    {
        Glib::RefPtr<Application> app = window->application();
        if (app) {
            header.set_subtitle(app->database()->get_path());
        }
    }
};

MainWindow::MainWindow(const std::tr1::shared_ptr<Repository>& repository)
    : m_priv(new Priv(repository))
{
    add(m_priv->welcome_screen);
    m_priv->header.set_title("Sound Collection");

    property_application().signal_changed().connect(
        sigc::bind(sigc::mem_fun(m_priv.get(), &Priv::application_changed), this));

    set_titlebar(m_priv->header);
    set_default_size(800, 600);
}

Glib::RefPtr<Application> MainWindow::application()
{
    Gtk::Application* app = property_application().get_value().operator->();
    Application* scapp = dynamic_cast<Application*>(app);
    g_debug("property_application: %p, cast to myapp: %p", app, scapp);
    return Glib::RefPtr<Application>::cast_dynamic(property_application().get_value());
}
}
