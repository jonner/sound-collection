/*
 * location-resource.h
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

#ifndef _SC_LOCATION_RESOURCE_H
#define _SC_LOCATION_RESOURCE_H

#include <gom/gom.h>

G_BEGIN_DECLS

#define SC_TYPE_LOCATION_RESOURCE (sc_location_resource_get_type())
#define SC_LOCATION_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(  \
        (obj), SC_TYPE_LOCATION_RESOURCE, ScLocationResource))
#define SC_LOCATION_RESOURCE_CONST(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(        \
        (obj), SC_TYPE_LOCATION_RESOURCE, ScLocationResource const))
#define SC_LOCATION_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST(             \
        (klass), SC_TYPE_LOCATION_RESOURCE, ScLocationResourceClass))
#define SC_IS_LOCATION_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), SC_TYPE_LOCATION_RESOURCE))
#define SC_IS_LOCATION_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), SC_TYPE_LOCATION_RESOURCE))
#define SC_LOCATION_RESOURCE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS(             \
        (obj), SC_TYPE_LOCATION_RESOURCE, ScLocationResourceClass))

typedef struct _ScLocationResource ScLocationResource;
typedef struct _ScLocationResourceClass ScLocationResourceClass;
typedef struct _ScLocationResourcePrivate ScLocationResourcePrivate;

struct _ScLocationResource {
    GomResource parent;

    ScLocationResourcePrivate* priv;
};

struct _ScLocationResourceClass {
    GomResourceClass parent_class;
};

GType sc_location_resource_get_type(void) G_GNUC_CONST;

gint64 sc_location_resource_get_id(const ScLocationResource* self);
const char* sc_location_resource_get_name(const ScLocationResource* self);
const char* sc_location_resource_get_country(const ScLocationResource* self);
gfloat sc_location_resource_get_latitude(const ScLocationResource* self);
gfloat sc_location_resource_get_longitude(const ScLocationResource* self);

G_END_DECLS

#endif /* _SC_LOCATION_RESOURCE_H */
