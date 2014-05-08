/*
 * species-resource.c
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

#include "species-resource.h"

#define SC_SPECIES_RESOURCE_GET_PRIVATE(object) \
    (G_TYPE_INSTANCE_GET_PRIVATE(               \
        (object), SC_TYPE_SPECIES_RESOURCE, ScSpeciesResourcePrivate))

struct _ScSpeciesResourcePrivate {
    int id;
    char* order;
    char* family;
    char* genus;
    char* species;
    char* common_name;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_ORDER,
    PROP_FAMILY,
    PROP_GENUS,
    PROP_SPECIES,
    PROP_COMMON_NAME
};

G_DEFINE_TYPE(ScSpeciesResource, sc_species_resource, GOM_TYPE_RESOURCE)

static void sc_species_resource_finalize(GObject* object)
{
    ScSpeciesResource* self = (ScSpeciesResource*)object;
    g_free(self->priv->order);
    g_free(self->priv->family);
    g_free(self->priv->genus);
    g_free(self->priv->species);
    g_free(self->priv->common_name);

    G_OBJECT_CLASS(sc_species_resource_parent_class)->finalize(object);
}

static void sc_species_resource_set_property(GObject* obj,
                                             guint property_id,
                                             const GValue* value,
                                             GParamSpec* pspec)
{
    ScSpeciesResource* self = SC_SPECIES_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        self->priv->id = g_value_get_int64(value);
        break;
    case PROP_ORDER:
        g_free(self->priv->order);
        self->priv->order = g_value_dup_string(value);
        break;
    case PROP_FAMILY:
        g_free(self->priv->family);
        self->priv->family = g_value_dup_string(value);
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_species_resource_get_property(GObject* obj,
                                             guint property_id,
                                             GValue* value,
                                             GParamSpec* pspec)
{
    ScSpeciesResource* self = SC_SPECIES_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        g_value_set_int64(value, self->priv->id);
        break;
    case PROP_ORDER:
        g_value_set_string(value, self->priv->order);
        break;
    case PROP_FAMILY:
        g_value_set_string(value, self->priv->family);
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_species_resource_class_init(ScSpeciesResourceClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = sc_species_resource_finalize;
    object_class->get_property = sc_species_resource_get_property;
    object_class->set_property = sc_species_resource_set_property;

    g_type_class_add_private(object_class, sizeof(ScSpeciesResourcePrivate));

    GomResourceClass* resource_class = GOM_RESOURCE_CLASS(klass);

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_ORDER,
        g_param_spec_string("order", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_FAMILY,
        g_param_spec_string("family", NULL, NULL, NULL, G_PARAM_READWRITE));

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

    gom_resource_class_set_table(resource_class, "species");
    gom_resource_class_set_primary_key(resource_class, "id");
}

static void sc_species_resource_init(ScSpeciesResource* self)
{
    self->priv = SC_SPECIES_RESOURCE_GET_PRIVATE(self);
}

