/*
 * location-tree-view.cc
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

#include "location-tree-view.h"
#include "util.h"

namespace SC {

struct LocationTreeView::Priv {
    Glib::RefPtr<LocationTreeModel> model;
    Gtk::TreeViewColumn id;
    Gtk::TreeViewColumn name;
    Gtk::TreeViewColumn latitude;
    Gtk::TreeViewColumn longitude;
    Gtk::TreeViewColumn country;

    Priv()
        : id("ID")
        , name("Name")
        , latitude("Latitude")
        , longitude("Longitude")
        , country("Country")
    {
    }
};

LocationTreeView::LocationTreeView()
    : m_priv(new Priv())
{
    append_column(m_priv->id);
    append_column(m_priv->name);
    append_column(m_priv->latitude);
    append_column(m_priv->longitude);
    append_column(m_priv->country);
}

void LocationTreeView::set_model(const Glib::RefPtr<LocationTreeModel>& model)
{
    m_priv->model = model;
    Gtk::TreeView::set_model(model);
    m_priv->id.clear();
    m_priv->name.clear();
    m_priv->latitude.clear();
    m_priv->longitude.clear();
    m_priv->country.clear();

    if (!model) {
        return;
    }

    m_priv->id.pack_start(model->columns().id);
    m_priv->name.pack_start(model->columns().name);
    m_priv->latitude.pack_start(model->columns().latitude);
    m_priv->longitude.pack_start(model->columns().longitude);
    m_priv->country.pack_start(model->columns().country);
}
}
