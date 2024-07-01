#include "tumble/query.h"
#include "tumble/shape.h"

#include <float.h>

int tmb_body_contains(const tmb_body *b, tmb_vec2 p)
{
    if (b->shape.kind == TMB_SHAPE_CIRCLE)
        return tmb_v2_len_sq(tmb_v2_sub(p, b->pos)) <=
               b->shape.radius * b->shape.radius;
    /* polygon: point is inside iff it is behind every edge plane (CCW) */
    tmb_vec2 local = tmb_xform_unpoint(b->xf, p);
    for (int i = 0; i < b->shape.count; i++) {
        float d = tmb_v2_dot(b->shape.normals[i],
                             tmb_v2_sub(local, b->shape.verts[i]));
        if (d > 0.0f)
            return 0;
    }
    return 1;
}

uint32_t tmb_world_query_point(tmb_world *w, tmb_vec2 p)
{
    for (size_t i = 0; i < tmb_world_body_count(w); i++)
        if (tmb_body_contains(tmb_world_body(w, (uint32_t)i), p))
            return (uint32_t)i;
    return UINT32_MAX;
}

size_t tmb_world_query_aabb(tmb_world *w, tmb_aabb box, uint32_t *out,
                            size_t cap)
{
    size_t found = 0;
    for (size_t i = 0; i < tmb_world_body_count(w); i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        tmb_aabb bb = tmb_shape_aabb(&b->shape, b->xf);
        if (tmb_aabb_overlap(box, bb)) {
            if (found < cap)
                out[found] = (uint32_t)i;
            found++;
        }
    }
    return found;
}

uint32_t tmb_world_nearest(tmb_world *w, tmb_vec2 p)
{
    uint32_t best = UINT32_MAX;
    float bestd = FLT_MAX;
    for (size_t i = 0; i < tmb_world_body_count(w); i++) {
        float d = tmb_v2_len_sq(tmb_v2_sub(tmb_world_body(w, (uint32_t)i)->pos, p));
        if (d < bestd) {
            bestd = d;
            best = (uint32_t)i;
        }
    }
    return best;
}
