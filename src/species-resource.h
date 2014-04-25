/*
 * species-resource.h
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

#ifndef _SC_SPECIES_RESOURCE_H
#define _SC_SPECIES_RESOURCE_H

#include <gom/gom.h>

G_BEGIN_DECLS

#define SC_TYPE_SPECIES_RESOURCE (sc_species_resource_get_type())
#define SC_SPECIES_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST( \
        (obj), SC_TYPE_SPECIES_RESOURCE, ScSpeciesResource))
#define SC_SPECIES_RESOURCE_CONST(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST(       \
        (obj), SC_TYPE_SPECIES_RESOURCE, ScSpeciesResource const))
#define SC_SPECIES_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST(            \
        (klass), SC_TYPE_SPECIES_RESOURCE, ScSpeciesResourceClass))
#define SC_IS_SPECIES_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), SC_TYPE_SPECIES_RESOURCE))
#define SC_IS_SPECIES_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), SC_TYPE_SPECIES_RESOURCE))
#define SC_SPECIES_RESOURCE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS(            \
        (obj), SC_TYPE_SPECIES_RESOURCE, ScSpeciesResourceClass))
typedef struct _ScSpeciesResource ScSpeciesResource;
typedef struct _ScSpeciesResourceClass ScSpeciesResourceClass;
typedef struct _ScSpeciesResourcePrivate ScSpeciesResourcePrivate;

struct _ScSpeciesResource {
    GomResource parent;

    ScSpeciesResourcePrivate* priv;
};

struct _ScSpeciesResourceClass {
    GomResourceClass parent_class;
};

GType sc_species_resource_get_type(void) G_GNUC_CONST;

G_END_DECLS

#endif /* _SC_SPECIES_RESOURCE_H */
