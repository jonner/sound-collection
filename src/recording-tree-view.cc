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
#include "util.h"

namespace SC {

static void duration_data_func(Gtk::CellRenderer* renderer, const Gtk::TreeModel::const_iterator& iter, const Glib::RefPtr<RecordingTreeModel>& model)
{
    float duration = iter->get_value(model->columns().duration);
    Gtk::CellRendererText* crt = dynamic_cast<Gtk::CellRendererText*>(renderer);

    crt->property_text() = format_duration(duration);
}

static void quality_data_func(Gtk::CellRenderer* renderer, const Gtk::TreeModel::const_iterator& iter, const Glib::RefPtr<RecordingTreeModel>& model, int q)
{
    float quality = iter->get_value(model->columns().quality);
    Gtk::CellRendererPixbuf* crp = dynamic_cast<Gtk::CellRendererPixbuf*>(renderer);

    if (!quality)
        crp->property_icon_name() = "";
    else
        crp->property_icon_name() = quality >= q ? "starred" : "non-starred";
}

struct RecordingTreeView::Priv {
    Glib::RefPtr<RecordingTreeModel> model;
    Gtk::TreeViewColumn id;
    Gtk::TreeViewColumn file;
    Gtk::CellRendererText file_renderer;
    Gtk::CellRendererText duration_renderer;
    Gtk::TreeViewColumn duration;
    Gtk::CellRendererPixbuf quality_renderer1;
    Gtk::CellRendererPixbuf quality_renderer2;
    Gtk::CellRendererPixbuf quality_renderer3;
    Gtk::CellRendererPixbuf quality_renderer4;
    Gtk::CellRendererPixbuf quality_renderer5;
    Gtk::TreeViewColumn quality;

    Priv()
        : id("ID")
        , file("File")
        , duration("Duration")
        , quality("Quality")
    {
        id.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        id.set_fixed_width(40);
        id.set_resizable(true);
        file.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        file.set_fixed_width(400);
        file.set_resizable(true);
        file_renderer.property_ellipsize() = Pango::ELLIPSIZE_START;
        duration.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        duration.set_fixed_width(100);
        duration.set_resizable(true);
        quality.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
        quality.set_fixed_width(100);
        quality.set_resizable(true);

        quality_renderer1.set_alignment(0.0, 0.5);
        quality_renderer2.set_alignment(0.0, 0.5);
        quality_renderer3.set_alignment(0.0, 0.5);
        quality_renderer4.set_alignment(0.0, 0.5);
        quality_renderer5.set_alignment(0.0, 0.5);
    }
};

RecordingTreeView::RecordingTreeView()
    : m_priv(new Priv())
{
    set_fixed_height_mode(true);
    append_column(m_priv->id);
    append_column(m_priv->file);
    append_column(m_priv->duration);
    append_column(m_priv->quality);
}

void RecordingTreeView::set_model(const Glib::RefPtr<RecordingTreeModel>& model)
{
    m_priv->model = model;
    Gtk::TreeView::set_model(model);
    m_priv->id.clear();
    m_priv->file.clear();
    m_priv->duration.clear();

    if (!model) {
        return;
    }

    m_priv->id.pack_start(model->columns().id);
    m_priv->file.pack_start(m_priv->file_renderer);
    m_priv->file.set_renderer(m_priv->file_renderer, model->columns().file);
    m_priv->duration.pack_start(m_priv->duration_renderer);
    m_priv->duration.set_renderer(m_priv->duration_renderer, model->columns().duration);
    m_priv->duration.set_cell_data_func(m_priv->duration_renderer, sigc::bind(sigc::ptr_fun(&duration_data_func), sigc::ref(model)));
    m_priv->quality.pack_start(m_priv->quality_renderer1, false);
    m_priv->quality.pack_start(m_priv->quality_renderer2, false);
    m_priv->quality.pack_start(m_priv->quality_renderer3, false);
    m_priv->quality.pack_start(m_priv->quality_renderer4, false);
    m_priv->quality.pack_start(m_priv->quality_renderer5, false);
    m_priv->quality.set_cell_data_func(m_priv->quality_renderer1, sigc::bind(sigc::ptr_fun(&quality_data_func), sigc::ref(model), 1));
    m_priv->quality.set_cell_data_func(m_priv->quality_renderer2, sigc::bind(sigc::ptr_fun(&quality_data_func), sigc::ref(model), 2));
    m_priv->quality.set_cell_data_func(m_priv->quality_renderer3, sigc::bind(sigc::ptr_fun(&quality_data_func), sigc::ref(model), 3));
    m_priv->quality.set_cell_data_func(m_priv->quality_renderer4, sigc::bind(sigc::ptr_fun(&quality_data_func), sigc::ref(model), 4));
    m_priv->quality.set_cell_data_func(m_priv->quality_renderer5, sigc::bind(sigc::ptr_fun(&quality_data_func), sigc::ref(model), 5));
}
}
