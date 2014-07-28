/*
 * location-tree-model.cc
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
#include "location-tree-model.h"

namespace SC {

// NULL-safe ustring creator
template <typename T>
static T make_string(const char* cstr)
{
    T str;
    if (cstr)
        str = cstr;
    return str;
}

enum {
    COLUMN_RESOURCE,
    COLUMN_ID,
    COLUMN_NAME,
    COLUMN_LATITUDE,
    COLUMN_LONGITUDE,
    COLUMN_COUNTRY
};

LocationModelColumns::LocationModelColumns()
{
    add(resource);
    add(id);
    add(name);
    add(latitude);
    add(longitude);
    add(country);
}

static const char* loading = "loading...";

struct LocationTreeModel::Priv {
    LocationModelColumns columns;
    WTF::GRefPtr<GomResourceGroup> locations;
    WTF::GRefPtr<ScLocationResource> empty_location;
    int stamp;

    Priv()
        : empty_location(adoptGRef(SC_LOCATION_RESOURCE(
              g_object_new(SC_TYPE_LOCATION_RESOURCE,
                           "name",
                           loading,
                           NULL))))
        , stamp(1)
    {
    }
};

const LocationModelColumns& LocationTreeModel::columns() const
{
    return m_priv->columns;
}

Glib::RefPtr<LocationTreeModel> LocationTreeModel::create()
{
    return Glib::RefPtr<LocationTreeModel>(new LocationTreeModel());
}

LocationTreeModel::LocationTreeModel()
    : Glib::ObjectBase(typeid(LocationTreeModel)) /* register custom GType */
    , Glib::Object() /* the GType is actually registered here */
    , Gtk::TreeModel()
    , m_priv(new Priv())
{
}

bool LocationTreeModel::path_deleted(const Gtk::TreeModel::Path& path)
{
    row_deleted(path);
    return false;
}

void LocationTreeModel::set_resource_group(GomResourceGroup* locations)
{
    foreach_path(sigc::mem_fun(this, &LocationTreeModel::path_deleted));
    m_priv->locations = locations;
    if (locations) {
        m_priv->stamp++;
        for (int i = 0; i < gom_resource_group_get_count(locations); ++i) {
            Gtk::TreeModel::Path path;
            path.push_back(i);
            row_inserted(path, make_iterator(i));
        }
    }
}

Gtk::TreeModelFlags LocationTreeModel::get_flags_vfunc(void) const
{
    return Gtk::TreeModelFlags(Gtk::TREE_MODEL_LIST_ONLY);
}

int LocationTreeModel::get_n_columns_vfunc(void) const
{
    return m_priv->columns.size();
}

GType LocationTreeModel::get_column_type_vfunc(int index) const
{
    g_return_val_if_fail(index >= 0, 0);
    g_return_val_if_fail(index < m_priv->columns.size(), 0);
    return m_priv->columns.types()[index];
}

static guint iter_get_index(const Gtk::TreeModel::iterator& iter)
{
    return GPOINTER_TO_INT(iter.gobj()->user_data);
}

static void iter_set_index(Gtk::TreeModel::iterator& iter, guint index)
{
    iter.gobj()->user_data = GINT_TO_POINTER(index);
}

#define SETUP_VALUE(T, ValBase, Val)                 \
    G_STMT_START                                     \
    {                                                \
        T::ValueType _local_val;                     \
        _local_val.init(T::ValueType::value_type()); \
        _local_val.set(Val);                         \
        ValBase.init(T::ValueType::value_type());    \
        ValBase = _local_val;                        \
    }                                                \
    G_STMT_END

void LocationTreeModel::get_value_vfunc(const iterator& iter,
                                        int column,
                                        Glib::ValueBase& value) const
{
    if (!iter)
        return;
    guint index = iter_get_index(iter);
    ScLocationResource* location = get_location(index);
    switch (column) {
    case COLUMN_RESOURCE:
        SETUP_VALUE(Gtk::TreeModelColumn<ScLocationResource*>,
                    value,
                    location);
        break;
    case COLUMN_ID:
        SETUP_VALUE(Gtk::TreeModelColumn<guint64>,
                    value,
                    sc_location_resource_get_id(location));
        break;
    case COLUMN_NAME:
        SETUP_VALUE(Gtk::TreeModelColumn<Glib::ustring>,
                    value,
                    make_string<Glib::ustring>(sc_location_resource_get_name(location)));
        break;
    case COLUMN_LATITUDE:
        SETUP_VALUE(Gtk::TreeModelColumn<float>,
                    value,
                    sc_location_resource_get_latitude(location));
        break;
    case COLUMN_LONGITUDE:
        SETUP_VALUE(Gtk::TreeModelColumn<float>,
                    value,
                    sc_location_resource_get_longitude(location));
        break;
    case COLUMN_COUNTRY:
        SETUP_VALUE(Gtk::TreeModelColumn<std::string>,
                    value,
                    make_string<std::string>(sc_location_resource_get_country(location)));
        break;
    default:
        g_warning("Invalid column %i", column);
    }
}

bool LocationTreeModel::iter_next_vfunc(const iterator& iter,
                                        iterator& iter_next) const
{
    guint index = iter_get_index(iter);
    iter_next = make_iterator(index + 1);
    return iter_next;
}

bool LocationTreeModel::iter_children_vfunc(const iterator& parent,
                                            iterator& iter) const
{
    iter = iterator(); /* invalid iter */
    return false;
}

bool LocationTreeModel::iter_has_child_vfunc(const iterator& iter) const
{
    return false;
}

int LocationTreeModel::iter_n_children_vfunc(const iterator& iter) const
{
    return 0;
}

int LocationTreeModel::iter_n_root_children_vfunc(void) const
{
    if (!m_priv->locations)
        return 0;

    return gom_resource_group_get_count(m_priv->locations.get());
}

bool LocationTreeModel::iter_nth_child_vfunc(const iterator& parent,
                                             int n,
                                             iterator& iter) const
{
    iter = iterator();
    return false;
}

bool LocationTreeModel::iter_nth_root_child_vfunc(int n,
                                                  iterator& iter) const
{
    iter = make_iterator(n);
    return iter;
}

bool LocationTreeModel::iter_parent_vfunc(const iterator& child,
                                          iterator& iter) const
{
    // list model only -- no parents
    iter = iterator();
    return false;
}

Gtk::TreeModel::Path LocationTreeModel::get_path_vfunc(const iterator& iter) const
{
    Gtk::TreeModel::Path path;
    guint index = iter_get_index(iter);
    path.push_back(index);
    return path;
}

bool LocationTreeModel::get_iter_vfunc(const Path& path,
                                       iterator& iter) const
{
    iter = iterator();
    if (path.size() != 1) {
        return false;
    }

    guint index = path[0];
    iter = make_iterator(index);
    return iter;
}

bool LocationTreeModel::iter_is_valid(const iterator& iter) const
{
    return (iter.get_stamp() == m_priv->stamp);
}

ScLocationResource* LocationTreeModel::get_location(guint index) const
{
    if (!m_priv->locations)
        return 0;
    ScLocationResource* location = SC_LOCATION_RESOURCE(
        gom_resource_group_get_index(m_priv->locations.get(), index));

    if (location) {
        return location;
    }
    g_debug("location %u not fetched, fetching...", index);
    fetch_location(index);
    return m_priv->empty_location.get();
}

Gtk::TreeModel::iterator LocationTreeModel::make_iterator(guint index) const
{
    iterator iter;
    if (m_priv->locations
        && index < gom_resource_group_get_count(m_priv->locations.get())) {
        iter.set_stamp(m_priv->stamp);
        iter_set_index(iter, index);
    }
    return iter;
}

struct FetchLocationData {
    LocationTreeModel* self;
    guint index;
    guint count;

    FetchLocationData(LocationTreeModel* self, guint index, guint count)
        : self(self)
        , index(index)
        , count(count)
    {
    }
};

void LocationTreeModel::location_fetch_done_proxy(GObject* source,
                                                  GAsyncResult* result,
                                                  gpointer user_data)
{
    FetchLocationData* data = reinterpret_cast<FetchLocationData*>(user_data);
    GomResourceGroup* locations = reinterpret_cast<GomResourceGroup*>(source);
    data->self->location_fetch_done(locations,
                                    result,
                                    data->index,
                                    data->count);
    delete data;
}

void LocationTreeModel::location_fetch_done(GomResourceGroup* locations,
                                            GAsyncResult* result,
                                            guint index,
                                            guint count)
{
    GError* error = 0;
    if (!gom_resource_group_fetch_finish(locations, result, &error)) {
        g_warning("Failed to fetch location %i: %s", index, error->message);
        g_clear_error(&error);
        return;
    }

    g_debug("Location %i fetched. signaling changed...", index);
    Gtk::TreeModel::Path path;
    path.push_back(index);
    row_changed(path, make_iterator(index));
}

void LocationTreeModel::fetch_location(guint index) const
{
    gom_resource_group_fetch_async(GOM_RESOURCE_GROUP(m_priv->locations.get()),
                                   index,
                                   1,
                                   LocationTreeModel::location_fetch_done_proxy,
                                   new FetchLocationData(const_cast<SC::LocationTreeModel*>(this),
                                                         index,
                                                         1));
}
}
