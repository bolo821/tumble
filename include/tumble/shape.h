#ifndef TUMBLE_SHAPE_H
#define TUMBLE_SHAPE_H

#include "tumble/vec2.h"
#include "tumble/transform.h"
#include "tumble/aabb.h"
#include "tumble/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TMB_MAX_POLY_VERTS 64

typedef enum {
    TMB_SHAPE_CIRCLE,
    TMB_SHAPE_POLYGON
} tmb_shape_kind;

/* A collision shape in body-local coordinates. Polygons are convex, wound
 * counter-clockwise, with precomputed outward edge normals. */
typedef struct {
    tmb_shape_kind kind;
    float radius;                        /* circle */
    int count;                           /* polygon vertex count */
    tmb_vec2 verts[TMB_MAX_POLY_VERTS];  /* polygon vertices */
    tmb_vec2 normals[TMB_MAX_POLY_VERTS];/* polygon edge normals */
} tmb_shape;

tmb_shape tmb_shape_circle(float radius);
tmb_shape tmb_shape_box(float half_w, float half_h);

/* Build a convex polygon from `n` vertices; reorders to CCW and computes edge
 * normals. Returns an error if n is out of range or the polygon is degenerate. */
tmb_err tmb_shape_polygon(tmb_shape *out, const tmb_vec2 *verts, int n);

/* Regular n-gon of the given circumradius. */
tmb_err tmb_shape_regular(tmb_shape *out, int sides, float radius);

/* World-space AABB of the shape under transform xf. */
tmb_aabb tmb_shape_aabb(const tmb_shape *s, tmb_xform xf);

/* Mass, rotational inertia (about the centroid) and centroid for a given
 * density. */
void tmb_shape_compute_mass(const tmb_shape *s, float density, float *mass,
                            float *inertia, tmb_vec2 *centroid);

/* The farthest point of the shape along direction d (local space) — the support
 * function used by collision. */
tmb_vec2 tmb_shape_support(const tmb_shape *s, tmb_vec2 d);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SHAPE_H */
