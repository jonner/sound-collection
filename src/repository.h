/*
 * repository.h
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

#ifndef _REPOSITORY_H
#define _REPOSITORY_H

#include <giomm.h>
#include <gom/gom.h>
#include <glibmm.h>
#include <tr1/memory>

namespace SC {
class Repository {
public:
    Repository(GomAdapter* adapter, const Glib::ustring& audio_path);

    void get_locations_async(const Gio::SlotAsyncReady& slot);
    GomResourceGroup* get_locations_finish(const Glib::RefPtr<Gio::AsyncResult>& result);
    GomRepository* cobj();
    sigc::signal<void>& signal_database_changed() const;
    void import_file_async(const Glib::RefPtr<Gio::File>& file,
                           const Gio::SlotAsyncReady& slot);
    bool import_file_finish(const Glib::RefPtr<Gio::AsyncResult>& result);
    Glib::RefPtr<Gio::File> audio_dir() const;

private:
    static void repository_migrate_finished_proxy(GObject* source_object,
                                                  GAsyncResult* res,
                                                  gpointer user_data);
    void repository_migrate_finished(GomRepository* repository,
                                     GAsyncResult* res);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};

}

#endif /* _REPOSITORY_H */

