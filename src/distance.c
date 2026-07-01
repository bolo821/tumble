#include "tumble/distance.h"
#include "tumble/geom.h"

#include <math.h>

tmb_vec2 tmb_shape_support_world(const tmb_shape *s, tmb_xform xf, tmb_vec2 d)
{
    if (s->kind == TMB_SHAPE_CIRCLE) {
        tmb_vec2 dir = tmb_v2_normalize(d);
        return tmb_v2_add(xf.pos, tmb_v2_scale(dir, s->radius));
    }
    /* rotate d into local space, find the extreme vertex, transform back */
    tmb_vec2 ld = tmb_v2(xf.c * d.x + xf.s * d.y, -xf.s * d.x + xf.c * d.y);
    int best = 0;
    float bd = tmb_v2_dot(s->verts[0], ld);
    for (int i = 1; i < s->count; i++) {
        float dot = tmb_v2_dot(s->verts[i], ld);
        if (dot > bd) {
            bd = dot;
            best = i;
        }
    }
    return tmb_xform_point(xf, s->verts[best]);
}

/* Closest point to the origin on segment ab, plus the barycentric weights. */
static tmb_vec2 closest_seg(tmb_vec2 a, tmb_vec2 b, float *wa, float *wb)
{
    tmb_vec2 ab = tmb_v2_sub(b, a);
    float t = -tmb_v2_dot(a, ab);
    float den = tmb_v2_dot(ab, ab);
    if (den < 1e-20f) {
        *wa = 1.0f;
        *wb = 0.0f;
        return a;
    }
    t /= den;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    *wa = 1.0f - t;
    *wb = t;
    return tmb_v2_add(a, tmb_v2_scale(ab, t));
}

typedef struct {
    tmb_vec2 p;   /* Minkowski difference point */
    tmb_vec2 a, b; /* supports on each shape */
} simplex_pt;

float tmb_shape_distance(const tmb_shape *sa, tmb_xform xa, const tmb_shape *sb,
                         tmb_xform xb, tmb_vec2 *pa, tmb_vec2 *pb)
{
    simplex_pt s[3];
    int n = 0;
    tmb_vec2 dir = tmb_v2_sub(xb.pos, xa.pos);
    if (tmb_v2_len_sq(dir) < 1e-12f)
        dir = tmb_v2(1, 0);

    for (int iter = 0; iter < 32; iter++) {
        tmb_vec2 supA = tmb_shape_support_world(sa, xa, tmb_v2_neg(dir));
        tmb_vec2 supB = tmb_shape_support_world(sb, xb, dir);
        simplex_pt w;
        w.a = supA;
        w.b = supB;
        w.p = tmb_v2_sub(supB, supA);

        /* if we cannot make progress toward the origin, we are done */
        if (n > 0 && tmb_v2_dot(w.p, dir) - tmb_v2_dot(s[0].p, dir) < 1e-6f)
            break;

        s[n++] = w;

        /* reduce the simplex to the feature closest to the origin */
        if (n == 1) {
            dir = tmb_v2_neg(s[0].p);
        } else if (n == 2) {
            float wa, wb;
            tmb_vec2 c = closest_seg(s[0].p, s[1].p, &wa, &wb);
            dir = tmb_v2_neg(c);
        } else { /* triangle: find the feature of the simplex closest to O */
            tmb_tri_region region;
            tmb_vec2 c = tmb_closest_point_on_triangle(
                tmb_v2(0, 0), s[0].p, s[1].p, s[2].p, &region);
            if (region == TMB_TRI_INSIDE)
                return 0.0f; /* origin enclosed -> shapes overlap */

            /* Reduce the simplex to the closest feature, then keep searching
             * toward the origin from it. Copy through a scratch simplex so the
             * reassignment cannot alias. */
            simplex_pt r[3];
            r[0] = s[0];
            r[1] = s[1];
            r[2] = s[2];
            switch (region) {
            case TMB_TRI_VERT_A: s[0] = r[0]; n = 1; break;
            case TMB_TRI_VERT_B: s[0] = r[1]; n = 1; break;
            case TMB_TRI_VERT_C: s[0] = r[2]; n = 1; break;
            case TMB_TRI_EDGE_AB: s[0] = r[0]; s[1] = r[1]; n = 2; break;
            case TMB_TRI_EDGE_BC: s[0] = r[1]; s[1] = r[2]; n = 2; break;
            case TMB_TRI_EDGE_CA: s[0] = r[2]; s[1] = r[0]; n = 2; break;
            default: n = 2; break;
            }
            dir = tmb_v2_neg(c);
        }
        if (tmb_v2_len_sq(dir) < 1e-12f)
            return 0.0f; /* origin on the boundary */
    }

    /* closest feature is s[0..n); recover the closest points */
    float wa = 1.0f, wb = 0.0f;
    tmb_vec2 closest;
    if (n >= 2)
        closest = closest_seg(s[0].p, s[1].p, &wa, &wb);
    else
        closest = s[0].p;
    if (pa)
        *pa = (n >= 2) ? tmb_v2_add(tmb_v2_scale(s[0].a, wa),
                                    tmb_v2_scale(s[1].a, wb))
                       : s[0].a;
    if (pb)
        *pb = (n >= 2) ? tmb_v2_add(tmb_v2_scale(s[0].b, wa),
                                    tmb_v2_scale(s[1].b, wb))
                       : s[0].b;
    return tmb_v2_len(closest);
}
