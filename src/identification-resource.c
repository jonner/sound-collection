/*
 * identification-resource.c
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

#include "identification-resource.h"

#define SC_IDENTIFICATION_RESOURCE_GET_PRIVATE(object)            \
    (G_TYPE_INSTANCE_GET_PRIVATE((object),                        \
                                 SC_TYPE_IDENTIFICATION_RESOURCE, \
                                 ScIdentificationResourcePrivate))

struct _ScIdentificationResourcePrivate {
    int id;
    int species_id;
    int recording_id;
    char* ssp;
    char* sound_type;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_SPECIES_ID,
    PROP_RECORDING_ID,
    PROP_SUBSPECIES,
    PROP_SOUND_TYPE
};

G_DEFINE_TYPE(ScIdentificationResource,
              sc_identification_resource,
              GOM_TYPE_RESOURCE)

static void sc_identification_resource_finalize(GObject* object)
{
    ScIdentificationResource* self = (ScIdentificationResource*)object;
    g_free(self->priv->ssp);
    g_free(self->priv->sound_type);

    G_OBJECT_CLASS(sc_identification_resource_parent_class)->finalize(object);
}

static void sc_identification_resource_set_property(GObject* obj,
                                                    guint property_id,
                                                    const GValue* value,
                                                    GParamSpec* pspec)
{
    ScIdentificationResource* self = SC_IDENTIFICATION_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        self->priv->id = g_value_get_int64(value);
        break;
    case PROP_SPECIES_ID:
        self->priv->species_id = g_value_get_int64(value);
        break;
    case PROP_RECORDING_ID:
        self->priv->recording_id = g_value_get_int64(value);
        break;
    case PROP_SUBSPECIES:
        g_free(self->priv->ssp);
        self->priv->ssp = g_value_dup_string(value);
        break;
    case PROP_SOUND_TYPE:
        g_free(self->priv->sound_type);
        self->priv->sound_type = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_identification_resource_get_property(GObject* obj,
                                                    guint property_id,
                                                    GValue* value,
                                                    GParamSpec* pspec)
{
    ScIdentificationResource* self = SC_IDENTIFICATION_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        g_value_set_int64(value, self->priv->id);
        break;
    case PROP_SPECIES_ID:
        g_value_set_int64(value, self->priv->species_id);
        break;
    case PROP_RECORDING_ID:
        g_value_set_int64(value, self->priv->recording_id);
        break;
    case PROP_SUBSPECIES:
        g_value_set_string(value, self->priv->ssp);
        break;
    case PROP_SOUND_TYPE:
        g_value_set_string(value, self->priv->sound_type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_identification_resource_class_init(
    ScIdentificationResourceClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = sc_identification_resource_finalize;
    object_class->get_property = sc_identification_resource_get_property;
    object_class->set_property = sc_identification_resource_set_property;

    g_type_class_add_private(object_class,
                             sizeof(ScIdentificationResourcePrivate));

    GomResourceClass* resource_class = GOM_RESOURCE_CLASS(klass);
    gom_resource_class_set_table(resource_class, "identification");
    gom_resource_class_set_primary_key(resource_class, "id");

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_SPECIES_ID,
        g_param_spec_int64(
            "species-id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));
    gom_resource_class_set_reference(
        resource_class, "species-id", "species", "id");

    g_object_class_install_property(
        object_class,
        PROP_RECORDING_ID,
        g_param_spec_int64(
            "recording-id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));
    gom_resource_class_set_reference(
        resource_class, "recording-id", "recordings", "id");

    g_object_class_install_property(
        object_class,
        PROP_SUBSPECIES,
        g_param_spec_string("subspecies", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_SOUND_TYPE,
        g_param_spec_string("sound-type", NULL, NULL, NULL, G_PARAM_READWRITE));
}

static void sc_identification_resource_init(ScIdentificationResource* self)
{
    self->priv = SC_IDENTIFICATION_RESOURCE_GET_PRIVATE(self);
}

