#ifndef TUMBLE_TRANSFORM_H
#define TUMBLE_TRANSFORM_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A rigid 2D transform: a position and a rotation stored as its sine/cosine so
 * applying it costs no trig. This maps a body's shape-local coordinates into
 * world space. */
typedef struct {
    tmb_vec2 pos;
    float s, c; /* sin and cos of the rotation angle */
} tmb_xform;

tmb_xform tmb_xform_identity(void);
tmb_xform tmb_xform_make(tmb_vec2 pos, float angle);
void tmb_xform_set_angle(tmb_xform *xf, float angle);

/* Apply to a point (rotate then translate) and to a direction (rotate only). */
tmb_vec2 tmb_xform_point(tmb_xform xf, tmb_vec2 local);
tmb_vec2 tmb_xform_dir(tmb_xform xf, tmb_vec2 dir);
/* Map a world point back into local coordinates. */
tmb_vec2 tmb_xform_unpoint(tmb_xform xf, tmb_vec2 world);

float tmb_clampf(float v, float lo, float hi);
/* Euclidean (floored) modulo: result is always in [0, m) for m > 0, so it is
 * safe for wrapping a possibly-negative index into an array bucket. */
int tmb_floor_mod(long v, int m);
float tmb_absf(float v);
float tmb_minf(float a, float b);
float tmb_maxf(float a, float b);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_TRANSFORM_H */
