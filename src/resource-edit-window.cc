/*
 * resource-edit-window.cc
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

#include "resource-edit-window.h"

namespace SC {

struct ResourceEditWindow::Priv {
    ResourceEditForm* form; // weak ref
    Gtk::Box layout;
    Gtk::InfoBar infobar;
    Gtk::Label info_label;
    Gtk::Spinner spinner;

    Priv()
        : form(0)
        , layout(Gtk::ORIENTATION_VERTICAL)
    {
        layout.show();
        layout.pack_start(infobar, FALSE, TRUE);
        infobar.set_message_type(Gtk::MESSAGE_INFO);
        Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
        dynamic_cast<Gtk::Container*>(infobar.get_content_area())->add(*box);
        box->show();
        box->pack_start(info_label);
        info_label.show();
        box->pack_start(spinner);
    }
};

ResourceEditWindow::ResourceEditWindow()
    : m_priv(new Priv())
{
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    add_button("Save", Gtk::RESPONSE_APPLY);
    signal_response().connect(sigc::mem_fun(this, &ResourceEditWindow::on_response));
    get_content_area()->pack_start(m_priv->layout, true, true);
}

void ResourceEditWindow::add_form(ResourceEditForm& form)
{
    m_priv->form = &form;
    form.property_margin() = 10;
    m_priv->layout.pack_start(form);
    form.show();
}

void ResourceEditWindow::on_resource_save_proxy(GObject* source, GAsyncResult* result, gpointer user_data)
{
    ResourceEditWindow* self = reinterpret_cast<ResourceEditWindow*>(user_data);
    self->on_resource_save(GOM_RESOURCE(source), result);
}

void ResourceEditWindow::on_resource_save(GomResource* resource, GAsyncResult* result)
{
    m_priv->spinner.stop();
    m_priv->spinner.hide();
    GError* error = 0;

    if (!gom_resource_save_finish(resource, result, &error)) {
        m_priv->infobar.set_message_type(Gtk::MESSAGE_ERROR);
        m_priv->info_label.set_text(Glib::ustring::compose("Failed to save resource: %1", error->message));
        g_clear_error(&error);
    } else {
        m_priv->info_label.set_text("Saved");
    }
    Glib::signal_timeout().connect_seconds_once(sigc::mem_fun(this, &Gtk::Widget::hide), 1);
}

void ResourceEditWindow::on_response(int response_id)
{
    set_response_sensitive(Gtk::RESPONSE_APPLY, false);
    set_response_sensitive(Gtk::RESPONSE_CANCEL, false);

    if (response_id == Gtk::RESPONSE_APPLY) {
        m_priv->info_label.set_text("Saving...");
        m_priv->spinner.start();
        m_priv->spinner.show();
        m_priv->infobar.show();
        gom_resource_save_async(m_priv->form->resource(), on_resource_save_proxy, this);
        return;
    }

    hide();
}
}
