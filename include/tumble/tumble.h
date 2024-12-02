#ifndef TUMBLE_H
#define TUMBLE_H

/* Tumble — a small 2D rigid-body physics engine. */

#include <stddef.h>
#include <stdint.h>

#include "tumble/errors.h"
#include "tumble/vec2.h"
#include "tumble/transform.h"
#include "tumble/aabb.h"
#include "tumble/shape.h"
#include "tumble/body.h"
#include "tumble/contact.h"
#include "tumble/joint.h"
#include "tumble/world.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TMB_VERSION_MAJOR 0
#define TMB_VERSION_MINOR 4
#define TMB_VERSION_PATCH 0
#define TMB_VERSION_STRING "0.4.0"

/* While loading untrusted scenes, cap the body and joint counts so a small
 * input cannot ask for an enormous simulation. */
#define TMB_SCENE_MAX_BODIES 8192u
#define TMB_SCENE_MAX_JOINTS 8192u

const char *tmb_version(void);

/* Load a scene (text or binary) into a freshly initialised world. */
tmb_err tmb_load_scene_text(const char *text, size_t len, tmb_world *out);
tmb_err tmb_load_scene_bin(const uint8_t *data, size_t len, tmb_world *out);
tmb_err tmb_load_file(const char *path, tmb_world *out);

/* Serialize a world. */
tmb_err tmb_write_scene_text(const tmb_world *w, tmb_buf *out);
tmb_err tmb_write_scene_bin(const tmb_world *w, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_H */
