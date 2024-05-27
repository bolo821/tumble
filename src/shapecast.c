#include "tumble/shapecast.h"
#include "tumble/distance.h"
#include "tumble/transform.h"

#include <math.h>

tmb_casthit tmb_world_circlecast(tmb_world *w, tmb_vec2 origin, tmb_vec2 dir,
                                 float r, float max_dist)
{
    tmb_casthit best;
    best.hit = 0;
    best.t = max_dist;
    best.body = UINT32_MAX;
    best.point = origin;
    best.normal = tmb_v2(0, 0);

    tmb_vec2 d = tmb_v2_normalize(dir);
    /* a one-point "circle" shape we advance along the ray */
    tmb_shape probe = tmb_shape_circle(r);

    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        tmb_body_sync(b);

        /* conservative advancement on GJK distance between the probe and the
         * body, marching from t=0 toward max_dist */
        float t = 0.0f;
        int hit = 0;
        for (int it = 0; it < 32 && t < best.t; it++) {
            tmb_xform px = tmb_xform_make(tmb_v2_add(origin, tmb_v2_scale(d, t)),
                                          0.0f);
            tmb_vec2 pa, pb;
            float dist =
                tmb_shape_distance(&probe, px, &b->shape, b->xf, &pa, &pb);
            if (dist <= 1e-3f) {
                hit = 1;
                best.hit = 1;
                best.t = t;
                best.body = (uint32_t)i;
                best.point = pb;
                tmb_vec2 nrm = tmb_v2_sub(pa, pb);
                best.normal = tmb_v2_normalize(nrm);
                break;
            }
            t += dist; /* safe step: cannot overshoot a collision */
        }
        (void)hit;
    }
    return best;
}
