/*
 * test-audio-player.cc
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

#include <gtkmm.h>
#include <gst/gst.h>
#include "simple-audio-player.h"

static void free_window(Gtk::Window* win)
{
    delete win;
}

typedef std::vector<Glib::RefPtr<Gio::File> > FileVector;
static void open(const FileVector& files,
                 const Glib::ustring& hint,
                 const Glib::RefPtr<Gtk::Application>& app)
{
    g_debug("%s: %i files", G_STRFUNC, files.size());
    if (files.empty())
        g_error("No file specified");

    for (FileVector::const_iterator it = files.begin();
         it != files.end();
         ++it) {
        g_debug("file: %s", (*it)->get_uri().c_str());
    }

    Glib::RefPtr<Gio::File> f = files[0];
    if (!f->query_exists()) {
        g_error("File %s does not exist", f->get_path().c_str());
        app->quit();
    }

    Gtk::Window* win = new Gtk::Window();
    SC::SimpleAudioPlayer* player = Gtk::manage(new SC::SimpleAudioPlayer(f));
    player->show();
    win->add(*player);
    app->add_window(*win);
    win->show();
    win->signal_hide().connect(sigc::bind(sigc::ptr_fun(free_window), win));
}

int main(int argc, char* argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.quotidian.TestAudioPlayer",
                                                                  Gio::APPLICATION_HANDLES_OPEN);
    gst_init(NULL, NULL);
    app->signal_open().connect(sigc::bind(sigc::ptr_fun(open), app));
    app->run(argc, argv);
    return 0;
}
