#ifndef TUMBLE_SVG_H
#define TUMBLE_SVG_H

#include "tumble/world.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Export a snapshot of the world as an SVG document. Built on the debug
 * renderer's decomposition (shapes, contacts, joints), it flips the y axis so
 * physics-up maps to screen-down and fits everything to a viewport. Handy for
 * embedding deterministic frames in docs and test reports. */

typedef struct {
    int width, height;     /* output pixel size */
    float padding;         /* world-units of margin around the content */
    int draw_aabbs;
    int draw_contacts;
    int draw_joints;
    float stroke;          /* stroke width in pixels */
} tmb_svg_opts;

tmb_svg_opts tmb_svg_defaults(void);
/* Render the world into `out` as a complete SVG document. */
tmb_err tmb_svg_export(tmb_world *w, const tmb_svg_opts *opts, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SVG_H */
