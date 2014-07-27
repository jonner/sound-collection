/*
 * location-tree-model.h
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

#ifndef _LOCATION_TREE_MODEL_H
#define _LOCATION_TREE_MODEL_H

#include <gom/gom.h>
#include <gtkmm.h>
#include <tr1/memory>
#include "location-resource.h"

namespace SC {

struct LocationModelColumns : public Gtk::TreeModel::ColumnRecord {
    Gtk::TreeModelColumn<ScLocationResource*> resource;
    Gtk::TreeModelColumn<gint64> id;
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<float> latitude;
    Gtk::TreeModelColumn<float> longitude;
    Gtk::TreeModelColumn<Glib::ustring> country;

    LocationModelColumns();
};

class LocationTreeModel : public Gtk::TreeModel,
                          public Glib::Object {
public:
    static Glib::RefPtr<LocationTreeModel> create();
    void set_resource_group(GomResourceGroup* locations);
    const LocationModelColumns& columns() const;

private:
    LocationTreeModel();

    //TreeModel vfuncs
    virtual Gtk::TreeModelFlags get_flags_vfunc(void) const;
    virtual int get_n_columns_vfunc(void) const;
    virtual GType get_column_type_vfunc(int index) const;
    virtual void get_value_vfunc(const iterator& iter, int column, Glib::ValueBase& value) const;
    virtual bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
    virtual bool iter_children_vfunc(const iterator& parent, iterator& iter) const;
    virtual bool iter_has_child_vfunc(const iterator& iter) const;
    virtual int iter_n_children_vfunc(const iterator& iter) const;
    virtual int iter_n_root_children_vfunc(void) const;
    virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
    virtual bool iter_nth_root_child_vfunc(int n, iterator& iter) const;
    virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
    virtual Path get_path_vfunc(const iterator& iter) const;
    virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;
    virtual bool iter_is_valid(const iterator& iter) const;

    ScLocationResource* get_location(guint index) const;
    void fetch_location(guint index) const;
    void invalidate_all();
    iterator make_iterator(guint index) const;
    bool path_deleted(const Gtk::TreeModel::Path& path);

    static void location_fetch_done_proxy(GObject* source,
                                          GAsyncResult* result,
                                          gpointer user_data);
    void location_fetch_done(GomResourceGroup* locations,
                             GAsyncResult* result,
                             guint index,
                             guint count);

    struct Priv;
    std::tr1::shared_ptr<Priv> m_priv;
};
}

#endif /* _LOCATION_TREE_MODEL_H */

