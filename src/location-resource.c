/*
 * location-resource.c
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

#include "location-resource.h"

#define SC_LOCATION_RESOURCE_GET_PRIVATE(object) \
    (G_TYPE_INSTANCE_GET_PRIVATE(                \
        (object), SC_TYPE_LOCATION_RESOURCE, ScLocationResourcePrivate))

struct _ScLocationResourcePrivate {
    gint64 id;
    char* name;
    gfloat latitude;
    gfloat longitude;
    char* country;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_NAME,
    PROP_LATITUDE,
    PROP_LONGITUDE,
    PROP_COUNTRY,
};

G_DEFINE_TYPE(ScLocationResource, sc_location_resource, GOM_TYPE_RESOURCE)

static void sc_location_resource_finalize(GObject* object)
{
    ScLocationResource* self = (ScLocationResource*)object;
    g_free(self->priv->name);
    g_free(self->priv->country);

    G_OBJECT_CLASS(sc_location_resource_parent_class)->finalize(object);
}

static void sc_location_resource_set_property(GObject* obj,
                                              guint property_id,
                                              const GValue* value,
                                              GParamSpec* pspec)
{
    ScLocationResource* self = SC_LOCATION_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        self->priv->id = g_value_get_int64(value);
        break;
    case PROP_NAME:
        g_free(self->priv->name);
        self->priv->name = g_value_dup_string(value);
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_location_resource_get_property(GObject* obj,
                                              guint property_id,
                                              GValue* value,
                                              GParamSpec* pspec)
{
    ScLocationResource* self = SC_LOCATION_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        g_value_set_int64(value, self->priv->id);
        break;
    case PROP_NAME:
        g_value_set_string(value, self->priv->name);
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
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_location_resource_class_init(ScLocationResourceClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = sc_location_resource_finalize;
    object_class->get_property = sc_location_resource_get_property;
    object_class->set_property = sc_location_resource_set_property;

    g_type_class_add_private(object_class, sizeof(ScLocationResourcePrivate));

    GomResourceClass* resource_class = GOM_RESOURCE_CLASS(klass);

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, 0, G_MAXINT64, 0, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_NAME,
        g_param_spec_string("name", NULL, NULL, NULL, G_PARAM_READWRITE));

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
        PROP_COUNTRY,
        g_param_spec_string("country", NULL, NULL, NULL, G_PARAM_READWRITE));

    gom_resource_class_set_table(resource_class, "locations");
    gom_resource_class_set_primary_key(resource_class, "id");
}

static void sc_location_resource_init(ScLocationResource* self)
{
    self->priv = SC_LOCATION_RESOURCE_GET_PRIVATE(self);
}

gint64 sc_location_resource_get_id(const ScLocationResource* self)
{
    return self->priv->id;
}

const char* sc_location_resource_get_name(const ScLocationResource* self)
{
    return self->priv->name;
}
const char* sc_location_resource_get_country(const ScLocationResource* self)
{
    return self->priv->country;
}

gfloat sc_location_resource_get_latitude(const ScLocationResource* self)
{
    return self->priv->latitude;
}

gfloat sc_location_resource_get_longitude(const ScLocationResource* self)
{
    return self->priv->longitude;
}

