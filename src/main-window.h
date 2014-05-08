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

class Application;

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();
    void set_repository(GomRepository* repository);

private:
    Glib::RefPtr<Application> application();
    void on_import_clicked();
    void on_import_file_done(const Glib::RefPtr<Gio::AsyncResult>& result);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}
#endif // _MAIN_WINDOW_H
