/*
 * recording-tree-view.cc
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

#include "recording-tree-view.h"

namespace SC {

struct RecordingTreeView::Priv {
    Glib::RefPtr<RecordingTreeModel> model;
    Gtk::TreeViewColumn id;
    Gtk::TreeViewColumn file;
    Gtk::TreeViewColumn location;
    Gtk::TreeViewColumn country;

    Priv()
        : id("ID")
        , file("File")
        , location("Location")
        , country("Country")
    {
        id.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        id.set_fixed_width(40);
        id.set_resizable(true);
        file.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        file.set_fixed_width(400);
        file.set_resizable(true);
        location.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        location.set_fixed_width(200);
        location.set_resizable(true);
        country.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        country.set_fixed_width(100);
        country.set_resizable(true);
    }
};

RecordingTreeView::RecordingTreeView()
    : m_priv(new Priv())
{
    set_fixed_height_mode(true);
    append_column(m_priv->id);
    append_column(m_priv->file);
    append_column(m_priv->location);
    append_column(m_priv->country);
}

void RecordingTreeView::set_model(const Glib::RefPtr<RecordingTreeModel>& model)
{
    m_priv->model = model;
    Gtk::TreeView::set_model(model);

    if (!model)
        return;

    m_priv->id.pack_start(model->columns().id);
    m_priv->file.pack_start(model->columns().file);
    m_priv->location.pack_start(model->columns().location);
    m_priv->country.pack_start(model->columns().country);
}
}
