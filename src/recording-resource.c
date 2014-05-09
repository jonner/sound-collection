/*
 * recording-resource.c
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

#include "recording-resource.h"

#define SC_RECORDING_RESOURCE_GET_PRIVATE(object) \
    (G_TYPE_INSTANCE_GET_PRIVATE(                 \
        (object), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourcePrivate))

struct _ScRecordingResourcePrivate {
    gint64 id;
    gfloat duration;
    gint quality;
    char* recordist;
    GDateTime* date;
    gint64 location_id;
    gfloat elevation;
    char* file;
    char* remarks;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_DURATION,
    PROP_QUALITY,
    PROP_RECORDIST,
    PROP_DATE,
    PROP_LOCATION_ID,
    PROP_ELEVATION,
    PROP_FILE,
    PROP_REMARKS
};

G_DEFINE_TYPE(ScRecordingResource, sc_recording_resource, GOM_TYPE_RESOURCE)

static void sc_recording_resource_finalize(GObject* object)
{
    ScRecordingResource* self = (ScRecordingResource*)object;
    g_free(self->priv->recordist);
    g_free(self->priv->file);
    g_free(self->priv->remarks);
    if (self->priv->date)
        g_date_time_unref(self->priv->date);

    G_OBJECT_CLASS(sc_recording_resource_parent_class)->finalize(object);
}

static void sc_recording_resource_set_property(GObject* obj,
                                               guint property_id,
                                               const GValue* value,
                                               GParamSpec* pspec)
{
    ScRecordingResource* self = SC_RECORDING_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        self->priv->id = g_value_get_int64(value);
        break;
    case PROP_DURATION:
        self->priv->duration = g_value_get_float(value);
        break;
    case PROP_QUALITY:
        self->priv->quality = g_value_get_int(value);
        break;
    case PROP_RECORDIST:
        g_free(self->priv->recordist);
        self->priv->recordist = g_value_dup_string(value);
        break;
    case PROP_LOCATION_ID:
        self->priv->location_id = g_value_get_int64(value);
        break;
    case PROP_ELEVATION:
        self->priv->elevation = g_value_get_float(value);
        break;
    case PROP_FILE:
        g_free(self->priv->file);
        self->priv->file = g_value_dup_string(value);
        break;
    case PROP_DATE:
        self->priv->date = g_value_dup_boxed(value);
        break;
    case PROP_REMARKS:
        g_free(self->priv->remarks);
        self->priv->remarks = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_recording_resource_get_property(GObject* obj,
                                               guint property_id,
                                               GValue* value,
                                               GParamSpec* pspec)
{
    ScRecordingResource* self = SC_RECORDING_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        g_value_set_int64(value, self->priv->id);
        break;
    case PROP_DURATION:
        g_value_set_float(value, self->priv->duration);
        break;
    case PROP_QUALITY:
        g_value_set_int(value, self->priv->quality);
        break;
    case PROP_RECORDIST:
        g_value_set_string(value, self->priv->recordist);
        break;
    case PROP_LOCATION_ID:
        g_value_set_int64(value, self->priv->location_id);
        break;
    case PROP_ELEVATION:
        g_value_set_float(value, self->priv->elevation);
        break;
    case PROP_FILE:
        g_value_set_string(value, self->priv->file);
        break;
    case PROP_DATE:
        g_value_set_boxed(value, self->priv->date);
        break;
    case PROP_REMARKS:
        g_value_set_string(value, self->priv->remarks);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_recording_resource_class_init(ScRecordingResourceClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = sc_recording_resource_finalize;
    object_class->get_property = sc_recording_resource_get_property;
    object_class->set_property = sc_recording_resource_set_property;

    g_type_class_add_private(object_class, sizeof(ScRecordingResourcePrivate));

    GomResourceClass* resource_class = GOM_RESOURCE_CLASS(klass);

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, 0, G_MAXINT64, 0, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_DURATION,
        g_param_spec_float(
            "duration", NULL, NULL, -G_MAXFLOAT, G_MAXFLOAT, -1, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_QUALITY,
        g_param_spec_int(
            "quality", NULL, NULL, -1, 5, -1, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_RECORDIST,
        g_param_spec_string("recordist", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_LOCATION_ID,
        g_param_spec_int64("location-id", NULL, NULL, 0, G_MAXINT64, 0, G_PARAM_READWRITE));
    gom_resource_class_set_reference(
        resource_class, "location-id", "locations", "id");

    g_object_class_install_property(
        object_class,
        PROP_DATE,
        g_param_spec_boxed(
            "date", NULL, NULL, G_TYPE_DATE_TIME, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_ELEVATION,
        g_param_spec_float(
            "elevation", NULL, NULL, -G_MAXFLOAT, G_MAXFLOAT, -G_MAXFLOAT, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_FILE,
        g_param_spec_string("file", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_REMARKS,
        g_param_spec_string("remarks", NULL, NULL, NULL, G_PARAM_READWRITE));

    gom_resource_class_set_table(resource_class, "recordings");
    gom_resource_class_set_primary_key(resource_class, "id");
}

static void sc_recording_resource_init(ScRecordingResource* self)
{
    self->priv = SC_RECORDING_RESOURCE_GET_PRIVATE(self);
}

gint64 sc_recording_resource_get_id(const ScRecordingResource* self)
{
    return self->priv->id;
}

const char* sc_recording_resource_get_file(const ScRecordingResource* self)
{
    return self->priv->file;
}

gint64 sc_recording_resource_get_location_id(const ScRecordingResource* self)
{
    return self->priv->location_id;
}

gint sc_recording_resource_get_quality(const ScRecordingResource* self)
{
    return self->priv->quality;
}

GDateTime* sc_recording_resource_get_date(const ScRecordingResource* self)
{
    return self->priv->date;
}

const char* sc_recording_resource_get_recordist(const ScRecordingResource* self)
{
    return self->priv->recordist;
}

gfloat sc_recording_resource_get_elevation(const ScRecordingResource* self)
{
    return self->priv->elevation;
}

const char* sc_recording_resource_get_remarks(const ScRecordingResource* self)
{
    return self->priv->remarks;
}

gfloat sc_recording_resource_get_duration(const ScRecordingResource* self)
{
    return self->priv->duration;
}


