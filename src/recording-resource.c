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
    int id;
    char* genus;
    char* species;
    char* common_name;
    GDateTime* date;
    char* sound_type;
    char* remarks;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_GENUS,
    PROP_SPECIES,
    PROP_COMMON_NAME,
    PROP_DATE,
    PROP_SOUND_TYPE,
    PROP_REMARKS
};

G_DEFINE_TYPE(ScRecordingResource, sc_recording_resource, GOM_TYPE_RESOURCE)

static void sc_recording_resource_finalize(GObject* object)
{
    ScRecordingResource* self = (ScRecordingResource*)object;
    g_free(self->priv->genus);
    g_free(self->priv->species);
    g_free(self->priv->common_name);
    g_free(self->priv->sound_type);
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
    case PROP_GENUS:
        g_free(self->priv->genus);
        self->priv->genus = g_value_dup_string(value);
        break;
    case PROP_SPECIES:
        g_free(self->priv->species);
        self->priv->species = g_value_dup_string(value);
        break;
    case PROP_COMMON_NAME:
        g_free(self->priv->common_name);
        self->priv->common_name = g_value_dup_string(value);
        break;
    case PROP_DATE:
        self->priv->date = g_value_dup_boxed(value);
        break;
    case PROP_SOUND_TYPE:
        g_free(self->priv->sound_type);
        self->priv->sound_type = g_value_dup_string(value);
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
    case PROP_GENUS:
        g_value_set_string(value, self->priv->genus);
        break;
    case PROP_SPECIES:
        g_value_set_string(value, self->priv->species);
        break;
    case PROP_COMMON_NAME:
        g_value_set_string(value, self->priv->common_name);
        break;
    case PROP_DATE:
        g_value_set_boxed(value, self->priv->date);
        break;
    case PROP_SOUND_TYPE:
        g_value_set_string(value, self->priv->sound_type);
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

    g_object_class_install_property(
        object_class,
        PROP_GENUS,
        g_param_spec_string("genus", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_SPECIES,
        g_param_spec_string("species", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_COMMON_NAME,
        g_param_spec_string("common-name", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_DATE,
        g_param_spec_boxed(
            "date", NULL, NULL, G_TYPE_DATE_TIME, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_SOUND_TYPE,
        g_param_spec_string("sound-type", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_REMARKS,
        g_param_spec_string("remarks", NULL, NULL, NULL, G_PARAM_READWRITE));
}

static void sc_recording_resource_init(ScRecordingResource* self)
{
    self->priv = SC_RECORDING_RESOURCE_GET_PRIVATE(self);
}

