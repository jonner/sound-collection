#ifndef __SC_RECORDING_RESOURCE_H__
#define __SC_RECORDING_RESOURCE_H__

#include <gom/gom.h>

G_BEGIN_DECLS

#define SC_TYPE_RECORDING_RESOURCE \
    (sc_recording_resource_get_type ())
#define SC_RECORDING_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResource))
#define SC_RECORDING_RESOURCE_CONST(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResource const))
#define SC_RECORDING_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourceClass))
#define SC_IS_RECORDING_RESOURCE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC_TYPE_RECORDING_RESOURCE))
#define SC_IS_RECORDING_RESOURCE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), SC_TYPE_RECORDING_RESOURCE))
#define SC_RECORDING_RESOURCE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), SC_TYPE_RECORDING_RESOURCE, ScRecordingResourceClass))

typedef struct _ScRecordingResource ScRecordingResource;
typedef struct _ScRecordingResourceClass ScRecordingResourceClass;
typedef struct _ScRecordingResourcePrivate ScRecordingResourcePrivate;

struct _ScRecordingResource
{
    GomResource parent;

    ScRecordingResourcePrivate *priv;
};

struct _ScRecordingResourceClass
{
    GomResourceClass parent_class;
};

GType sc_recording_resource_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __SC_RECORDING_RESOURCE_H__ */
