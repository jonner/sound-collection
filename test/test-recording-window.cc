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
#include <gst/gst.h>
#include <tr1/memory>

#include "recording-window.h"
#include "recording.h"
#include "repository.h"

static GomAdapter* adapter = 0;
static std::tr1::shared_ptr<SC::Repository> repository;
static gint64 id = 0;

int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.quotidian.TestRecordingWindow");
    gst_init(NULL, NULL);
    if (argc < 2) {
        g_warning("Usage: %s RECORDINGID", argv[0]);
        return -1;
    }

    id = atoi(argv[1]);

    Glib::RefPtr<Gio::File> f = Gio::File::create_for_path("sound-collection.sqlite");
    adapter = gom_adapter_new();
    GError* error = 0;
    if (!gom_adapter_open_sync(adapter, f->get_uri().c_str(), &error)) {
        g_error("Unable to open adapter: %s", error->message);
        g_clear_error(&error);
        return -1;
    }
    repository.reset(new SC::Repository(adapter));
    GValue id_value = G_VALUE_INIT;
    g_value_init(&id_value, G_TYPE_INT64);
    g_value_set_int64(&id_value, id);
    GomFilter* filter = gom_filter_new_eq(SC_TYPE_RECORDING_RESOURCE, "id", &id_value);
    GomResource* recording = gom_repository_find_one_sync(repository->cobj(), SC_TYPE_RECORDING_RESOURCE, filter, &error);
    if (!recording) {
        g_error("Couldn't find recording %i: %s", id, error->message);
        g_clear_error(&error);
        return -1;
    }

    SC::RecordingWindow win(SC::Recording::create(SC_RECORDING_RESOURCE(recording)), repository);
    app->run(win, 1, argv);

    return 0;
}
