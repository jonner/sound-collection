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

#include "GRefPtr.h"
#include "main-window.h"
#include "recording-list-box.h"

namespace SC {
struct MainWindow::Priv {
    Priv()
        : layout(Gtk::ORIENTATION_VERTICAL)
        , import_button("Import")
    {
        header.set_title("Sound Collection");
        header.set_show_close_button(true);
        header.show();
    }

    Gtk::Box layout;
    Gtk::HeaderBar header;
    RecordingListBox recording_list;
    Gtk::Button import_button;
    WTF::GRefPtr<GomRepository> repository;
};

MainWindow::MainWindow()
    : m_priv(new Priv())
{
    add(m_priv->layout);
    m_priv->layout.pack_start(m_priv->recording_list, true, true);
    m_priv->layout.pack_start(m_priv->import_button, false, true);
    m_priv->layout.show_all();
    set_titlebar(m_priv->header);
    set_default_size(400, 400);
}

void MainWindow::set_repository(GomRepository* repository)
{
    m_priv->repository = repository;
    m_priv->recording_list.set_repository(repository);
}
}
