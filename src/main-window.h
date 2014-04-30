/*
 * main-window.h
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

#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include <gom/gom.h>
#include <gtkmm.h>
#include <tr1/memory>

namespace SC {

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();
    void set_repository(GomRepository* repository);

private:
    void on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

    static void got_results_proxy(GObject* source,
                                  GAsyncResult* result,
                                  gpointer user_data);
    void got_results(GomRepository* repository, GAsyncResult* result);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}
#endif // _MAIN_WINDOW_H
