/*
 * recording-list.cc
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

#include "application.h"
#include "GRefPtr.h"
#include "recording-list.h"
#include "recording-tree-model.h"
#include "recording-tree-view.h"
#include "recording-window.h"

namespace SC {
struct RecordingList::Priv {
    Gtk::ScrolledWindow scroller;
    Glib::RefPtr<RecordingTreeModel> tree_model;
    RecordingTreeView tree_view;
    std::tr1::shared_ptr<Repository> repository;

    Priv(const std::tr1::shared_ptr<Repository>& repository)
        : tree_model(RecordingTreeModel::create())
        , repository(repository)
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
                                  SC_TYPE_RECORDING_RESOURCE,
                                  0 /*m_priv->filter.get()*/,
                                  Priv::got_recordings_proxy,
                                  this);
    }

    static void got_recordings_proxy(GObject* source,
                                     GAsyncResult* result,
                                     gpointer user_data)
    {
        GomRepository* repository = reinterpret_cast<GomRepository*>(source);
        Priv* self = reinterpret_cast<Priv*>(user_data);
        self->got_recordings(repository, result);
    }

    void got_recordings(GomRepository* repository, GAsyncResult* result)
    {
        g_debug("%s", G_STRFUNC);
        GError* error = 0;
        WTF::GRefPtr<GomResourceGroup> results = adoptGRef(gom_repository_find_finish(repository, result, &error));
        if (error) {
            g_warning("Unable to find resources: %s", error->message);
            g_clear_error(&error);
            return;
        }

        // FIXME: figure out why we need to unset the model before setting it
        tree_view.set_model(Glib::RefPtr<RecordingTreeModel>());
        g_debug("total results: %i", gom_resource_group_get_count(results.get()));
        tree_model->set_resource_group(results.get());
        tree_view.set_model(tree_model);
    }

    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn* column)
    {
        Gtk::TreeModel::iterator iter = tree_model->get_iter(path);
        ScRecordingResource* resource = (*iter)[tree_model->columns().resource];
        std::tr1::shared_ptr<Recording> recording = Recording::create(resource);
        RecordingWindow::display(recording, repository);
    }
};

RecordingList::RecordingList(const std::tr1::shared_ptr<Repository>& repository)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    , m_priv(new Priv(repository))
{
    pack_start(m_priv->scroller);
}
}
