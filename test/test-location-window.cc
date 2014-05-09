/*
 * test-recording-window.cc
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
#include <gom/gom.h>
#include "location-window.h"
#include "location.h"

static GomAdapter* adapter = 0;
static GomRepository* repository = 0;
static gint64 id = 0;

int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.quotidian.TestLocationWindow");
    if (argc > 2) {
        g_warning("Usage: %s [LOCATIONID]", argv[0]);
        return -1;
    }

    GomResource* recording = 0;
    Glib::RefPtr<Gio::File> f = Gio::File::create_for_path("sound-collection.sqlite");
    adapter = gom_adapter_new();
    GError* error = 0;
    if (!gom_adapter_open_sync(adapter, f->get_uri().c_str(), &error)) {
        g_error("Unable to open adapter: %s", error->message);
        g_clear_error(&error);
        return -1;
    }
    repository = gom_repository_new(adapter);

    if (argc == 2) {
        id = atoi(argv[1]);

        GValue id_value = G_VALUE_INIT;
        g_value_init(&id_value, G_TYPE_INT64);
        g_value_set_int64(&id_value, id);
        GomFilter* filter = gom_filter_new_eq(SC_TYPE_LOCATION_RESOURCE, "id", &id_value);
        recording = gom_repository_find_one_sync(repository, SC_TYPE_LOCATION_RESOURCE, filter, &error);
        if (!recording) {
            g_error("Couldn't find recording %i: %s", id, error->message);
            g_clear_error(&error);
            return -1;
        }
    } else {
        recording = GOM_RESOURCE(g_object_new(SC_TYPE_LOCATION_RESOURCE,
                                              "repository",
                                              repository,
                                              NULL));
    }

    SC::LocationWindow win(SC::Location::create(SC_LOCATION_RESOURCE(recording)));
    app->run(win, 1, argv);

    return 0;
}
