/*
 * equipment-resource.c
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

#include "equipment-resource.h"

#define SC_EQUIPMENT_RESOURCE_GET_PRIVATE(object) \
    (G_TYPE_INSTANCE_GET_PRIVATE(                 \
        (object), SC_TYPE_EQUIPMENT_RESOURCE, ScEquipmentResourcePrivate))

struct _ScEquipmentResourcePrivate {
    gint64 id;
    char* name;
    char* description;
    EquipmentCategory category;
};

enum {
    PROP_0,
    PROP_ID,
    PROP_NAME,
    PROP_DESCRIPTION,
    PROP_CATEGORY,
};

G_DEFINE_TYPE(ScEquipmentResource, sc_equipment_resource, GOM_TYPE_RESOURCE)

static void sc_equipment_resource_finalize(GObject* object)
{
    ScEquipmentResource* self = (ScEquipmentResource*)object;
    g_free(self->priv->name);
    g_free(self->priv->description);

    G_OBJECT_CLASS(sc_equipment_resource_parent_class)->finalize(object);
}

static void sc_equipment_resource_set_property(GObject* obj,
                                               guint property_id,
                                               const GValue* value,
                                               GParamSpec* pspec)
{
    ScEquipmentResource* self = SC_EQUIPMENT_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        self->priv->id = g_value_get_int64(value);
        break;
    case PROP_NAME:
        g_free(self->priv->name);
        self->priv->name = g_value_dup_string(value);
        break;
    case PROP_DESCRIPTION:
        g_free(self->priv->description);
        self->priv->description = g_value_dup_string(value);
        break;
    case PROP_CATEGORY:
        self->priv->category = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_equipment_resource_get_property(GObject* obj,
                                               guint property_id,
                                               GValue* value,
                                               GParamSpec* pspec)
{
    ScEquipmentResource* self = SC_EQUIPMENT_RESOURCE(obj);

    switch (property_id) {
    case PROP_ID:
        g_value_set_int64(value, self->priv->id);
        break;
    case PROP_NAME:
        g_value_set_string(value, self->priv->name);
        break;
    case PROP_DESCRIPTION:
        g_value_set_string(value, self->priv->description);
        break;
    case PROP_CATEGORY:
        g_value_set_float(value, self->priv->category);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, property_id, pspec);
    }
}

static void sc_equipment_resource_class_init(ScEquipmentResourceClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = sc_equipment_resource_finalize;
    object_class->get_property = sc_equipment_resource_get_property;
    object_class->set_property = sc_equipment_resource_set_property;

    g_type_class_add_private(object_class, sizeof(ScEquipmentResourcePrivate));

    GomResourceClass* resource_class = GOM_RESOURCE_CLASS(klass);

    g_object_class_install_property(
        object_class,
        PROP_ID,
        g_param_spec_int64(
            "id", NULL, NULL, -1, G_MAXINT64, -1, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_NAME,
        g_param_spec_string("name", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_DESCRIPTION,
        g_param_spec_string("description", NULL, NULL, NULL, G_PARAM_READWRITE));

    g_object_class_install_property(
        object_class,
        PROP_CATEGORY,
        g_param_spec_int(
            "category", NULL, NULL, 1, EQUIPMENT_CATEGORY_LAST, 1, G_PARAM_READWRITE));

    gom_resource_class_set_table(resource_class, "equipment");
    gom_resource_class_set_primary_key(resource_class, "id");
}

static void sc_equipment_resource_init(ScEquipmentResource* self)
{
    self->priv = SC_EQUIPMENT_RESOURCE_GET_PRIVATE(self);
}

gint64 sc_equipment_resource_get_id(const ScEquipmentResource* self)
{
    return self->priv->id;
}

const char* sc_equipment_resource_get_name(const ScEquipmentResource* self)
{
    return self->priv->name;
}

const char* sc_equipment_resource_get_description(const ScEquipmentResource* self)
{
    return self->priv->description;
}

EquipmentCategory sc_equipment_resource_get_category(const ScEquipmentResource* self)
{
    return self->priv->category;
}

