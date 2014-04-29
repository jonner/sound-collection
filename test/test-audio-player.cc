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

int main(int argc, char* argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.quotidian.TestAudioPlayer");
//    gst_init(&argc, &argv);
gst_init(NULL, NULL);

    Gtk::Window win;
    SC::SimpleAudioPlayer player(Gio::File::create_for_path("/home/jjongsma/Recordings/Originals/JMJ-20140418-113226-140418_40-USA-MN-GreyCloudDunes-VESP-3-song.flac"));
    player.show();
    win.add(player);
    win.show();
    
    app->run(win, argc, argv);
    return 0;
}
