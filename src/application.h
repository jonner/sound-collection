/*
 * application.h
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

#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <gio/gio.h>
#include <glib-object.h>
#include <gom/gom.h>
#include <gtkmm.h>
#include <tr1/memory>

namespace SC {
class Application : public Gtk::Application {
public:
    static Glib::RefPtr<SC::Application> create();
    Glib::RefPtr<const Gio::File> database() const;
    void import_file_async(const Glib::RefPtr<Gio::File>& file,
                           const Gio::SlotAsyncReady& slot);
    bool import_file_finish(const Glib::RefPtr<Gio::AsyncResult>& result);

private:
    Application();
    static void adapter_open_ready_proxy(GObject* source_object,
                                         GAsyncResult* res,
                                         gpointer user_data);
    void adapter_open_ready(GomAdapter* adapter, GAsyncResult* res);
    static void repository_migrate_finished_proxy(GObject* source_object,
                                                  GAsyncResult* res,
                                                  gpointer user_data);
    void repository_migrate_finished(GomRepository* repository,
                                     GAsyncResult* res);

    virtual int on_command_line(
        const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);
    void show();
    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif // _APPLICATION_H
