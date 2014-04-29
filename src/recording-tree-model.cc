/*
 * recording-tree-model.cc
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
#include "recording-tree-model.h"

namespace SC {

static const guint PAGE_SIZE = 30;

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
    COLUMN_ID,
    COLUMN_LENGTH,
    COLUMN_QUALITY,
    COLUMN_RECORDIST,
    COLUMN_DATE,
    COLUMN_LOCATION,
    COLUMN_LATITUDE,
    COLUMN_LONGITUDE,
    COLUMN_COUNTRY,
    COLUMN_ELEVATION,
    COLUMN_FILE,
    COLUMN_REMARKS
};

RecordingModelColumns::RecordingModelColumns()
{
    add(id);
    add(length);
    add(quality);
    add(recordist);
    add(date);
    add(location);
    add(latitude);
    add(longitude);
    add(country);
    add(elevation);
    add(file);
    add(remarks);
}

static const char* loading = "loading...";

struct RecordingTreeModel::Priv {
    RecordingModelColumns columns;
    WTF::GRefPtr<GomResourceGroup> recordings;
    WTF::GRefPtr<ScRecordingResource> empty_recording;
    int max_fetched;
    int stamp;

    Priv()
        : empty_recording(SC_RECORDING_RESOURCE(
              g_object_new(SC_TYPE_RECORDING_RESOURCE,
                           "file",
                           loading,
                           "location",
                           loading,
                           "country",
                           loading,
                           NULL)))
        , max_fetched(-1)
        , stamp(1)
    {
    }
};

const RecordingModelColumns& RecordingTreeModel::columns() const
{
    return m_priv->columns;
}

Glib::RefPtr<RecordingTreeModel> RecordingTreeModel::create()
{
    return Glib::RefPtr<RecordingTreeModel>(new RecordingTreeModel());
}

RecordingTreeModel::RecordingTreeModel()
    : Glib::ObjectBase(typeid(RecordingTreeModel)) /* register custom GType */
    , Glib::Object() /* the GType is actually registered here */
    , Gtk::TreeModel()
    , m_priv(new Priv())
{
}

bool RecordingTreeModel::path_deleted(const Gtk::TreeModel::Path& path)
{
    row_deleted(path);
    return false;
}

void RecordingTreeModel::set_resource_group(GomResourceGroup* recordings)
{
    foreach_path(sigc::mem_fun(this, &RecordingTreeModel::path_deleted));
    m_priv->recordings = recordings;
    if (recordings) {
        m_priv->stamp++;
        for (int i = 0; i < gom_resource_group_get_count(recordings); ++i) {
            Gtk::TreeModel::Path path;
            path.push_back(i);
            row_inserted(path, make_iterator(i));
        }
    }
}

Gtk::TreeModelFlags RecordingTreeModel::get_flags_vfunc(void) const
{
    return Gtk::TreeModelFlags(Gtk::TREE_MODEL_LIST_ONLY);
}

int RecordingTreeModel::get_n_columns_vfunc(void) const
{
    return m_priv->columns.size();
}

GType RecordingTreeModel::get_column_type_vfunc(int index) const
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

void RecordingTreeModel::get_value_vfunc(const iterator& iter,
                                         int column,
                                         Glib::ValueBase& value) const
{
    if (!iter)
        return;
    guint index = iter_get_index(iter);
    ScRecordingResource* recording = get_recording(index);
    g_debug("Getting value for index %i, column %i", index, column);
    switch (column) {
    case COLUMN_ID: {
        guint id = -1;
        g_object_get(recording, "id", &id, NULL);
        SETUP_VALUE(Gtk::TreeModelColumn<guint64>, value, id);
        break;
    }
    case COLUMN_LENGTH: {
        float length = 0;
        g_object_get(recording, "length", &length, NULL);
        SETUP_VALUE(Gtk::TreeModelColumn<float>, value, length);
        break;
    }
    case COLUMN_QUALITY:
        break;
    case COLUMN_RECORDIST:
        break;
    case COLUMN_DATE: {
        GDateTime* gdate = 0;
        g_object_get(recording, "date", &gdate, NULL);
        Glib::DateTime date = Glib::wrap(gdate);
        SETUP_VALUE(Gtk::TreeModelColumn<Glib::DateTime>, value, date);
        break;
    } break;
    case COLUMN_LOCATION:
        SETUP_VALUE(Gtk::TreeModelColumn<Glib::ustring>, value, make_string<Glib::ustring>(sc_recording_resource_get_location(recording)));
        break;
    case COLUMN_LATITUDE:
        SETUP_VALUE(Gtk::TreeModelColumn<float>, value, sc_recording_resource_get_latitude(recording));
        break;
    case COLUMN_LONGITUDE:
        SETUP_VALUE(Gtk::TreeModelColumn<float>, value, sc_recording_resource_get_longitude(recording));
        break;
    case COLUMN_COUNTRY:
        SETUP_VALUE(Gtk::TreeModelColumn<Glib::ustring>, value, make_string<Glib::ustring>(sc_recording_resource_get_country(recording)));
        break;
    case COLUMN_ELEVATION:
        break;
    case COLUMN_FILE: {
        SETUP_VALUE(Gtk::TreeModelColumn<std::string>, value, make_string<std::string>(sc_recording_resource_get_file(recording)));
        break;
    }
    case COLUMN_REMARKS:
        break;
    default:
        g_warning("Invalid column %i", column);
    }
}

bool RecordingTreeModel::iter_next_vfunc(const iterator& iter,
                                         iterator& iter_next) const
{
    guint index = iter_get_index(iter);
    iter_next = make_iterator(index + 1);
    return iter_next;
}

bool RecordingTreeModel::iter_children_vfunc(const iterator& parent,
                                             iterator& iter) const
{
    iter = iterator(); /* invalid iter */
    return false;
}

bool RecordingTreeModel::iter_has_child_vfunc(const iterator& iter) const
{
    return false;
}

int RecordingTreeModel::iter_n_children_vfunc(const iterator& iter) const
{
    return 0;
}

int RecordingTreeModel::iter_n_root_children_vfunc(void) const
{
    if (!m_priv->recordings)
        return 0;

    return gom_resource_group_get_count(m_priv->recordings.get());
}

bool RecordingTreeModel::iter_nth_child_vfunc(const iterator& parent,
                                              int n,
                                              iterator& iter) const
{
    iter = iterator();
    return false;
}

bool RecordingTreeModel::iter_nth_root_child_vfunc(int n,
                                                   iterator& iter) const
{
    iter = make_iterator(n);
    return iter;
}

bool RecordingTreeModel::iter_parent_vfunc(const iterator& child,
                                           iterator& iter) const
{
    // list model only -- no parents
    iter = iterator();
    return false;
}

Gtk::TreeModel::Path RecordingTreeModel::get_path_vfunc(const iterator& iter) const
{
    Gtk::TreeModel::Path path;
    guint index = iter_get_index(iter);
    path.push_back(index);
    return path;
}

bool RecordingTreeModel::get_iter_vfunc(const Path& path,
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

bool RecordingTreeModel::iter_is_valid(const iterator& iter) const
{
    return (iter.get_stamp() == m_priv->stamp);
}

ScRecordingResource* RecordingTreeModel::get_recording(guint index) const
{
    if (!m_priv->recordings)
        return 0;
    ScRecordingResource* recording = SC_RECORDING_RESOURCE(
        gom_resource_group_get_index(m_priv->recordings.get(), index));

    if (recording) {
        g_debug("%s: recording %u already fetched, returning...", G_STRFUNC, index);
        return recording;
    }
    g_debug("%s: recording %u not fetched, fetching...", G_STRFUNC, index);
    fetch_recording(index);
    return m_priv->empty_recording.get();
}

Gtk::TreeModel::iterator RecordingTreeModel::make_iterator(guint index) const
{
    iterator iter;
    if (m_priv->recordings
        && index < gom_resource_group_get_count(m_priv->recordings.get())) {
        iter.set_stamp(m_priv->stamp);
        iter_set_index(iter, index);
    }
    return iter;
}

struct FetchRecordingData {
    RecordingTreeModel* self;
    guint index;
    guint count;

    FetchRecordingData(RecordingTreeModel* self, guint index, guint count)
        : self(self)
        , index(index)
        , count(count)
    {
    }
};

void RecordingTreeModel::recording_fetch_done_proxy(GObject* source,
                                                    GAsyncResult* result,
                                                    gpointer user_data)
{
    FetchRecordingData* data = reinterpret_cast<FetchRecordingData*>(user_data);
    GomResourceGroup* recordings = reinterpret_cast<GomResourceGroup*>(source);
    data->self->recording_fetch_done(recordings,
                                     result,
                                     data->index,
                                     data->count);
    delete data;
}

void RecordingTreeModel::recording_fetch_done(GomResourceGroup* recordings,
                                              GAsyncResult* result,
                                              guint index,
                                              guint count)
{
    GError* error = 0;
    if (!gom_resource_group_fetch_finish(recordings, result, &error)) {
        g_warning("Failed to fetch recording %i: %s", index, error->message);
        g_clear_error(&error);
        return;
    }

    m_priv->max_fetched = index + count;
    g_debug("Set max_fetched to %u", m_priv->max_fetched);
    Gtk::TreeModel::Path path;
    path.push_back(index);
    row_changed(path, make_iterator(index));
}

void RecordingTreeModel::fetch_recording(guint index) const
{
    // fetch the next /page that includes this index
    guint end = std::min(((index % PAGE_SIZE) + 1) * PAGE_SIZE,
                         gom_resource_group_get_count(m_priv->recordings.get()));
    guint start = std::max(0, m_priv->max_fetched);
    gom_resource_group_fetch_async(GOM_RESOURCE_GROUP(m_priv->recordings.get()),
                                   index,
                                   1,
                                   RecordingTreeModel::recording_fetch_done_proxy,
                                   new FetchRecordingData(const_cast<SC::RecordingTreeModel*>(this),
                                                          index,
                                                          1));
}
}
