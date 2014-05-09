/*
 * resource-edit-window.h
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

#ifndef _RESOURCE_EDIT_WINDOW_H
#define _RESOURCE_EDIT_WINDOW_H

#include <gtkmm.h>
#include <tr1/memory>

#include "resource-edit-form.h"

namespace SC {

class ResourceEditWindow : public Gtk::Dialog {
public:
    ResourceEditWindow();
    
    void add_form(ResourceEditForm& form);

private:
    void on_response(int response_id);
    static void on_resource_save_proxy(GObject* soruce, GAsyncResult* result, gpointer user_data);
    void on_resource_save(GomResource* resource, GAsyncResult* result);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _RESOURCE_EDIT_WINDOW_H */

