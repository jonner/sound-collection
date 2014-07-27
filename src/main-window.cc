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

#include "application.h"
#include "GRefPtr.h"
#include "import-dialog.h"
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
        header.set_show_close_button(true);
        header.show();
        scroller.add(tree_view);
        tree_view.signal_row_activated().connect(
            sigc::mem_fun(this, &Priv::on_row_activated));
    }

    void application_changed(MainWindow* window)
    {
        Glib::RefPtr<Application> app = window->application();
        if (app) {
            header.set_subtitle(app->database()->get_path());
            app->signal_database_changed().connect(sigc::mem_fun(this, &Priv::refresh_view));
        }
    }

    void refresh_view()
    {
        gom_repository_find_async(repository.get(),
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
        RecordingWindow::display(recording);
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
    m_priv->header.set_title("Sound Collection");
    property_application().signal_changed().connect(
        sigc::bind(sigc::mem_fun(m_priv.get(), &Priv::application_changed), this));

    set_titlebar(m_priv->header);
    set_default_size(800, 600);

    m_priv->import_button.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_import_clicked));
}

void MainWindow::set_repository(GomRepository* repository)
{
    m_priv->repository = repository;

    m_priv->refresh_view();
}

Glib::RefPtr<Application> MainWindow::application()
{
    Gtk::Application* app = property_application().get_value().operator->();
    Application* scapp = dynamic_cast<Application*>(app);
    g_debug("property_application: %p, cast to myapp: %p", app, scapp);
    return Glib::RefPtr<Application>::cast_dynamic(property_application().get_value());
}

void MainWindow::on_import_file_done(const Glib::RefPtr<Gio::AsyncResult>& result)
{
    try
    {
        application()->import_file_finish(result);
    }
    catch (const Glib::Error& error)
    {
        g_warning("failed to import file: %s", error.what().c_str());
    }
}

void MainWindow::on_import_clicked()
{
    ImportDialog* chooser = new ImportDialog(*this);
    chooser->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    chooser->add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
    if (chooser->run() == Gtk::RESPONSE_ACCEPT) {
        std::vector<Glib::RefPtr<Gio::File> > files = chooser->get_files();
        for (std::vector<Glib::RefPtr<Gio::File> >::iterator it = files.begin();
             it != files.end();
             ++it) {
            application()->import_file_async(*it,
                                             sigc::mem_fun(this, &MainWindow::on_import_file_done));
        }
    }
    chooser->hide();
    delete chooser;
}
}
