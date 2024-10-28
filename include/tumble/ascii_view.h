#ifndef TUMBLE_ASCII_VIEW_H
#define TUMBLE_ASCII_VIEW_H

#include "tumble/world.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Render the world to a width x height ASCII grid (a top-down snapshot, y up),
 * filled by point-sampling each body's shape. Rows are appended to out. */
tmb_err tmb_render_ascii(tmb_world *w, int width, int height, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_ASCII_VIEW_H */
