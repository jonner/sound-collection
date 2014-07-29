/*
 * application.cc
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

#include <gio/gio.h>
#include <gom/gom.h>
#include <gst/gst.h>

#include "application.h"
#include "equipment-resource.h"
#include "GRefPtr.h"
#include "identification-resource.h"
#include "location-resource.h"
#include "main-window.h"
#include "species-resource.h"
#include "task.h"

namespace SC {

const char* DB_NAME = "sound-collection.sqlite";
const char* AUDIO_DIR = "audio";

struct Application::Priv {
    int status;
    Glib::RefPtr<Gio::File> base;
    WTF::GRefPtr<GomAdapter> adapter;
    std::tr1::shared_ptr<Repository> repository;

    Priv()
        : status(0)
    {
    }

    void setup_collection_directory(const std::string& base_path)
    {
        base = Gio::File::create_for_path(base_path);
        try
        {
            base->make_directory_with_parents();
            base->get_child(AUDIO_DIR)->make_directory();
        }
        catch (const Gio::Error& e)
        {
            if (!(e.domain() == G_IO_ERROR && e.code() == Gio::Error::EXISTS))
                g_error("Unable to setup collection directory: %s(%i) - %s", g_quark_to_string(e.domain()), e.code(), e.what().c_str());
        }

        if (base->query_file_type() != Gio::FILE_TYPE_DIRECTORY)
            g_error("Collection path %s is not a directory", base->get_path().c_str());
    }
};

Glib::RefPtr<Application> Application::create()
{
    return Glib::RefPtr<Application>(new Application());
}

Application::Application()
    : Gtk::Application("org.quotidian.SoundCollection",
                       Gio::APPLICATION_FLAGS_NONE)
    , m_priv(new Priv())
{
}

Application::~Application()
{
    GError* error = 0;
    if (!gom_adapter_close_sync(m_priv->adapter.get(), &error)) {
        g_warning("Unable to close adapter: %s", error->message);
        g_clear_error(&error);
        return;
    }
}

void Application::on_startup()
{
    Gio::Application::on_startup();
    gst_init(NULL, NULL);
    std::string collection_path = Glib::getenv("COLLECTION_BASE");
    if (collection_path.empty())
        collection_path = Glib::build_filename(Glib::get_user_data_dir(), "SoundCollection");
    m_priv->setup_collection_directory(collection_path);

    std::string uri = database()->get_uri();
    g_debug("Opening db %s...", uri.c_str());

    m_priv->adapter = adoptGRef(gom_adapter_new());
    gom_adapter_open_async(m_priv->adapter.get(),
                           uri.c_str(),
                           Application::adapter_open_ready_proxy,
                           this);
}

void Application::on_activate()
{
    hold();
}

void Application::adapter_open_ready(GomAdapter* adapter, GAsyncResult* res)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    if (!gom_adapter_open_finish(adapter, res, &error)) {
        g_warning("failed to open adapter: %s", error->message);
        g_error_free(error);
        m_priv->status = -1;
        quit();
    } else
        g_debug("Opened adapter");

    m_priv->repository.reset(new Repository(m_priv->adapter.get()));
    show();
    release();
}

void Application::adapter_open_ready_proxy(GObject* source_object,
                                           GAsyncResult* res,
                                           gpointer user_data)
{
    Application* self = static_cast<Application*>(user_data);
    GomAdapter* adapter = reinterpret_cast<GomAdapter*>(source_object);

    self->adapter_open_ready(adapter, res);
}


void Application::show()
{
    MainWindow* win = new MainWindow(m_priv->repository);
    add_window(*win);
    win->show();
}

Glib::RefPtr<const Gio::File> Application::base() const
{
    return m_priv->base;
}

Glib::RefPtr<const Gio::File> Application::database() const
{
    g_return_val_if_fail(m_priv->base, Glib::RefPtr<const Gio::File>());
    return m_priv->base->get_child(DB_NAME);
}
}
