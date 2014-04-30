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
#include "recording-tree-model.h"
#include "recording-tree-view.h"
#include "recording-window.h"

namespace SC {
struct MainWindow::Priv {
    Priv()
        : layout(Gtk::ORIENTATION_VERTICAL)
        , import_button("Import")
        , tree_model(RecordingTreeModel::create())
    {
        header.set_title("Sound Collection");
        header.set_show_close_button(true);
        header.show();
        scroller.add(tree_view);
    }

    Gtk::Box layout;
    Gtk::HeaderBar header;
    Gtk::ScrolledWindow scroller;
    Glib::RefPtr<RecordingTreeModel> tree_model;
    RecordingTreeView tree_view;
    Gtk::Button import_button;
    WTF::GRefPtr<GomRepository> repository;
};

MainWindow::MainWindow()
    : m_priv(new Priv())
{
    add(m_priv->layout);
    m_priv->layout.pack_start(m_priv->scroller, true, true);
    m_priv->layout.pack_start(m_priv->import_button, false, true);
    m_priv->layout.show_all();
    set_titlebar(m_priv->header);
    set_default_size(800, 600);
}

void MainWindow::set_repository(GomRepository* repository)
{
    m_priv->repository = repository;

    gom_repository_find_async(m_priv->repository.get(),
                              SC_TYPE_RECORDING_RESOURCE,
                              0 /*m_priv->filter.get()*/,
                              MainWindow::got_results_proxy,
                              this);
}

void MainWindow::got_results_proxy(GObject* source,
                                   GAsyncResult* result,
                                   gpointer user_data)
{
    GomRepository* repository = reinterpret_cast<GomRepository*>(source);
    MainWindow* self = reinterpret_cast<MainWindow*>(user_data);
    self->got_results(repository, result);
}

void MainWindow::got_results(GomRepository* repository, GAsyncResult* result)
{
    g_debug("%s", G_STRFUNC);
    GError* error = 0;
    GomResourceGroup* results = gom_repository_find_finish(repository, result, &error);
    if (error) {
        g_warning("Unable to find resources: %s", error->message);
        g_clear_error(&error);
        return;
    }

    g_debug("total results: %i", gom_resource_group_get_count(results));
    m_priv->tree_model->set_resource_group(results);
    m_priv->tree_view.set_model(m_priv->tree_model);
    m_priv->tree_view.signal_row_activated().connect(
        sigc::mem_fun(this, &MainWindow::on_row_activated));
}

void MainWindow::on_row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column)
{
    Gtk::TreeModel::iterator iter = m_priv->tree_model->get_iter(path);
    ScRecordingResource* resource = (*iter)[m_priv->tree_model->columns().resource];
    std::tr1::shared_ptr<Recording> recording = Recording::create(resource);
    RecordingWindow::display(recording);
}
}
