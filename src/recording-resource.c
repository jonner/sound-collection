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

#include <glib.h>

#include "recording-resource.h"

#define SC_RECORDING_RESOURCE_GET_PRIVATE(object) \
    (G_TYPE_INSTANCE_GET_PRIVATE(                 \
        (object), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourcePrivate))

struct _ScRecordingResourcePrivate {
    gint64 id;
    gfloat length;
    gint quality;
    char* recordist;
    GDateTime* date;
    char* location;
    gfloat latitude;
    gfloat longitude;
    char* country;
    gfloat elevation;
    char* file;
    char* remarks;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_LENGTH,
    PROP_QUALITY,
    PROP_RECORDIST,
    PROP_DATE,
    PROP_LOCATION,
    PROP_LATITUDE,
    PROP_LONGITUDE,
    PROP_COUNTRY,
    PROP_ELEVATION,
    PROP_FILE,
    PROP_REMARKS
};

G_DEFINE_TYPE(ScRecordingResource, sc_recording_resource, GOM_TYPE_RESOURCE)

static void sc_recording_resource_finalize(GObject* object)
{
    ScRecordingResource* self = (ScRecordingResource*)object;
    g_free(self->priv->recordist);
    g_free(self->priv->location);
    g_free(self->priv->country);
    g_free(self->priv->file);
    g_free(self->priv->remarks);
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
    case PROP_LENGTH:
        self->priv->length = g_value_get_float(value);
        break;
    case PROP_QUALITY:
        self->priv->quality = g_value_get_int(value);
        break;
    case PROP_RECORDIST:
        g_free(self->priv->recordist);
        self->priv->recordist = g_value_dup_string(value);
        break;
    case PROP_LOCATION:
        g_free(self->priv->location);
        self->priv->location = g_value_dup_string(value);
        break;
    case PROP_LATITUDE:
        self->priv->latitude = g_value_get_float(value);
        break;
    case PROP_LONGITUDE:
        self->priv->longitude = g_value_get_float(value);
        break;
    case PROP_COUNTRY:
        g_free(self->priv->country);
        self->priv->country = g_value_dup_string(value);
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
    case PROP_LENGTH:
        g_value_set_float(value, self->priv->length);
        break;
    case PROP_QUALITY:
        g_value_set_int(value, self->priv->quality);
        break;
    case PROP_RECORDIST:
        g_value_set_string(value, self->priv->recordist);
        break;
    case PROP_LOCATION:
        g_value_set_string(value, self->priv->location);
        break;
    case PROP_LATITUDE:
        g_value_set_float(value, self->priv->latitude);
        break;
    case PROP_LONGITUDE:
        g_value_set_float(value, self->priv->longitude);
        break;
    case PROP_COUNTRY:
        g_value_set_string(value, self->priv->country);
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
    gom_resource_class_set_table(resource_class, "recordings");
    gom_resource_class_set_primary_key(resource_class, "id");

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));

    /*


    PROP_ELEVATION,
    PROP_FILE,
    PROP_REMARKS
    */

    g_object_class_install_property(
        object_class,
        PROP_LENGTH,
        g_param_spec_float(
            "length", NULL, NULL, -G_MAXFLOAT, G_MAXFLOAT, -1, G_PARAM_READWRITE));

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
        PROP_LATITUDE,
        g_param_spec_float(
            "latitude", NULL, NULL, -G_MAXFLOAT, G_MAXFLOAT, -G_MAXFLOAT, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_LONGITUDE,
        g_param_spec_float(
            "longitude", NULL, NULL, -G_MAXFLOAT, G_MAXFLOAT, -G_MAXFLOAT, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_LOCATION,
        g_param_spec_string("location", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_COUNTRY,
        g_param_spec_string("country", NULL, NULL, NULL, G_PARAM_READWRITE));

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
}

static void sc_recording_resource_init(ScRecordingResource* self)
{
    self->priv = SC_RECORDING_RESOURCE_GET_PRIVATE(self);
}

