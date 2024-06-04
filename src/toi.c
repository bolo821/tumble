#include "tumble/toi.h"
#include "tumble/distance.h"
#include "tumble/transform.h"

#include <math.h>

#define TMB_TOI_MAX_ITERS 32

static tmb_xform sweep_xform(const tmb_body *b, tmb_vec2 v, float t)
{
    tmb_xform xf;
    xf.pos = tmb_v2_add(b->pos, tmb_v2_scale(v, t));
    xf.s = b->xf.s;
    xf.c = b->xf.c;
    return xf;
}

tmb_toi_result tmb_toi(const tmb_body *a, tmb_vec2 va, const tmb_body *b,
                       tmb_vec2 vb, float dt, float target)
{
    tmb_toi_result r;
    r.t = 1.0f;
    r.hit = 0;
    r.point = a->pos;
    r.iterations = 0;

    if (target < 1e-4f)
        target = 1e-4f;
    tmb_vec2 rel = tmb_v2_sub(tmb_v2_scale(vb, dt), tmb_v2_scale(va, dt));
    float rel_speed = tmb_v2_len(rel);
    if (rel_speed < 1e-6f) {
        /* no relative translation: just test the initial configuration */
        tmb_vec2 pa, pb;
        float d = tmb_shape_distance(&a->shape, a->xf, &b->shape, b->xf, &pa,
                                     &pb);
        if (d <= target) {
            r.hit = 1;
            r.t = 0.0f;
            r.point = tmb_v2_scale(tmb_v2_add(pa, pb), 0.5f);
        }
        return r;
    }

    float t = 0.0f;
    for (int it = 0; it < TMB_TOI_MAX_ITERS; it++) {
        r.iterations = it + 1;
        tmb_xform xa = sweep_xform(a, va, t * dt);
        tmb_xform xb = sweep_xform(b, vb, t * dt);
        tmb_vec2 pa, pb;
        float d = tmb_shape_distance(&a->shape, xa, &b->shape, xb, &pa, &pb);
        if (d <= target) {
            r.hit = 1;
            r.t = t;
            r.point = tmb_v2_scale(tmb_v2_add(pa, pb), 0.5f);
            return r;
        }
        /* advance by the conservative amount: closing distance over the
         * maximum closing speed along the separation normal */
        float advance = (d - target) / rel_speed;
        t += advance;
        if (t >= 1.0f)
            return r; /* no impact within the step */
    }
    /* ran out of iterations very close to contact */
    r.t = t < 1.0f ? t : 1.0f;
    return r;
}
