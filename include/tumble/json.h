#ifndef TUMBLE_JSON_H
#define TUMBLE_JSON_H

#include "tumble/world.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Export a world to a JSON document: world settings, then an array of bodies
 * (type, pose, velocity, material and shape) and joints. Intended for tooling,
 * inspectors and golden-file tests rather than as a loader format -- the
 * binary and text scene formats remain the canonical inputs. */

tmb_err tmb_json_export(const tmb_world *w, tmb_buf *out);
/* Export just the body array (no enclosing object), for embedding. */
tmb_err tmb_json_export_bodies(const tmb_world *w, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_JSON_H */
