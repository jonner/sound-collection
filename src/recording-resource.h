/*
 * recording-resource.h
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

#ifndef __SC_RECORDING_RESOURCE_H__
#define __SC_RECORDING_RESOURCE_H__

#include <gom/gom.h>

G_BEGIN_DECLS

#define SC_TYPE_RECORDING_RESOURCE (sc_recording_resource_get_type())
#define SC_RECORDING_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(   \
        (obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResource))
#define SC_RECORDING_RESOURCE_CONST(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(         \
        (obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResource const))
#define SC_RECORDING_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST(              \
        (klass), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourceClass))
#define SC_IS_RECORDING_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), SC_TYPE_RECORDING_RESOURCE))
#define SC_IS_RECORDING_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), SC_TYPE_RECORDING_RESOURCE))
#define SC_RECORDING_RESOURCE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS(              \
        (obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourceClass))

typedef struct _ScRecordingResource ScRecordingResource;
typedef struct _ScRecordingResourceClass ScRecordingResourceClass;
typedef struct _ScRecordingResourcePrivate ScRecordingResourcePrivate;

struct _ScRecordingResource {
    GomResource parent;

    ScRecordingResourcePrivate* priv;
};

struct _ScRecordingResourceClass {
    GomResourceClass parent_class;
};

GType sc_recording_resource_get_type(void) G_GNUC_CONST;

gint64 sc_recording_resource_get_id(ScRecordingResource* self);
const char* sc_recording_resource_get_file(ScRecordingResource* self);
const char* sc_recording_resource_get_location(ScRecordingResource* self);
const char* sc_recording_resource_get_country(ScRecordingResource* self);
float sc_recording_resource_get_latitude(ScRecordingResource* self);
float sc_recording_resource_get_longitude(ScRecordingResource* self);

G_END_DECLS

#endif /* __SC_RECORDING_RESOURCE_H__ */
