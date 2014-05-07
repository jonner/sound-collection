/*
 * equipment-resource.h
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

#ifndef _SC_EQUIPMENT_RESOURCE_H
#define _SC_EQUIPMENT_RESOURCE_H

#include <gom/gom.h>

G_BEGIN_DECLS

#define SC_TYPE_EQUIPMENT_RESOURCE (sc_equipment_resource_get_type())
#define SC_EQUIPMENT_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(   \
        (obj), SC_TYPE_EQUIPMENT_RESOURCE, ScEquipmentResource))
#define SC_EQUIPMENT_RESOURCE_CONST(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(         \
        (obj), SC_TYPE_EQUIPMENT_RESOURCE, ScEquipmentResource const))
#define SC_EQUIPMENT_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST(              \
        (klass), SC_TYPE_EQUIPMENT_RESOURCE, ScEquipmentResourceClass))
#define SC_IS_EQUIPMENT_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), SC_TYPE_EQUIPMENT_RESOURCE))
#define SC_IS_EQUIPMENT_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), SC_TYPE_EQUIPMENT_RESOURCE))
#define SC_EQUIPMENT_RESOURCE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS(              \
        (obj), SC_TYPE_EQUIPMENT_RESOURCE, ScEquipmentResourceClass))

typedef struct _ScEquipmentResource ScEquipmentResource;
typedef struct _ScEquipmentResourceClass ScEquipmentResourceClass;
typedef struct _ScEquipmentResourcePrivate ScEquipmentResourcePrivate;

struct _ScEquipmentResource {
    GomResource parent;

    ScEquipmentResourcePrivate* priv;
};

struct _ScEquipmentResourceClass {
    GomResourceClass parent_class;
};

GType sc_equipment_resource_get_type(void) G_GNUC_CONST;

typedef enum {
    EQUIPMENT_CATEGORY_RECORDER = 1,
    EQUIPMENT_CATEGORY_MICROPHONE,
    EQUIPMENT_CATEGORY_OTHER,
    EQUIPMENT_CATEGORY_LAST = EQUIPMENT_CATEGORY_OTHER,
} EquipmentCategory;

gint64 sc_equipment_resource_get_id(const ScEquipmentResource* self);
const char* sc_equipment_resource_get_name(const ScEquipmentResource* self);
const char* sc_equipment_resource_get_description(const ScEquipmentResource* self);
EquipmentCategory sc_equipment_resource_get_category(const ScEquipmentResource* self);

G_END_DECLS

#endif /* _SC_EQUIPMENT_RESOURCE_H */
