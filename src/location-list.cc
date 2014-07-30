/*
 * location-list.cc
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

#include "GRefPtr.h"
#include "location-list.h"
#include "location-tree-model.h"
#include "location-tree-view.h"

namespace SC {

struct LocationList::Priv {
    std::tr1::shared_ptr<Repository> repository;
    Glib::RefPtr<LocationTreeModel> model;
    Gtk::ScrolledWindow scroller;
    LocationTreeView tree_view;

    Priv(const std::tr1::shared_ptr<Repository>& repository)
        : repository(repository)
        , model(LocationTreeModel::create())
    {
        scroller.add(tree_view);
        tree_view.signal_row_activated().connect(
            sigc::mem_fun(this, &Priv::on_row_activated));
        scroller.show();
        tree_view.show();
        refresh_view();

        repository->signal_database_changed().connect(
            sigc::mem_fun(this, &Priv::refresh_view));
    }

    void refresh_view()
    {
        gom_repository_find_async(repository->cobj(),
                                  SC_TYPE_LOCATION_RESOURCE,
                                  0 /*m_priv->filter.get()*/,
                                  Priv::got_locations_proxy,
                                  this);
    }

    static void got_locations_proxy(GObject* source,
                                    GAsyncResult* result,
                                    gpointer user_data)
    {
        GomRepository* repository = reinterpret_cast<GomRepository*>(source);
        Priv* self = reinterpret_cast<Priv*>(user_data);
        self->got_locations(repository, result);
    }

    void got_locations(GomRepository* repository, GAsyncResult* result)
    {
        g_debug("%s", G_STRFUNC);
        GError* error = 0;
        WTF::GRefPtr<GomResourceGroup> results = adoptGRef(gom_repository_find_finish(repository, result, &error));
        if (error) {
            g_warning("Unable to find locations: %s", error->message);
            g_clear_error(&error);
            return;
        }

        // FIXME: figure out why we need to unset the model before setting it
        tree_view.set_model(Glib::RefPtr<LocationTreeModel>());
        g_debug("total locations: %i", gom_resource_group_get_count(results.get()));
        model->set_resource_group(results.get());
        tree_view.set_model(model);
    }

    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn* column)
    {
        Gtk::TreeModel::iterator iter = model->get_iter(path);
        ScLocationResource* resource = (*iter)[model->columns().resource];
        std::tr1::shared_ptr<Location> location = Location::create(resource);
        //RecordingWindow::display(recording, repository);
    }
};

LocationList::LocationList(const std::tr1::shared_ptr<Repository>& repository)
    : m_priv(new Priv(repository))
{
    pack_start(m_priv->scroller);
}
}
