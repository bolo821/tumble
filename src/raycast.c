#include "tumble/raycast.h"

#include <math.h>
#include <float.h>

int tmb_ray_circle(tmb_ray r, tmb_vec2 center, float radius, float *t,
                   tmb_vec2 *normal)
{
    tmb_vec2 m = tmb_v2_sub(r.origin, center);
    float a = tmb_v2_dot(r.dir, r.dir);
    if (a < 1e-20f)
        return 0;
    float b = 2.0f * tmb_v2_dot(m, r.dir);
    float c = tmb_v2_dot(m, m) - radius * radius;
    float disc = b * b - 4.0f * a * c;
    if (disc < 0.0f)
        return 0;
    float sq = sqrtf(disc);
    float t0 = (-b - sq) / (2.0f * a);
    if (t0 < 0.0f)
        t0 = (-b + sq) / (2.0f * a);
    if (t0 < 0.0f)
        return 0;
    if (r.max_t > 0.0f && t0 > r.max_t)
        return 0;
    *t = t0;
    tmb_vec2 hit = tmb_v2_add(r.origin, tmb_v2_scale(r.dir, t0));
    if (normal)
        *normal = tmb_v2_normalize(tmb_v2_sub(hit, center));
    return 1;
}

int tmb_ray_polygon(tmb_ray r, const tmb_shape *s, tmb_xform xf, float *t,
                    tmb_vec2 *normal)
{
    /* transform the ray into the polygon's local space */
    tmb_vec2 o = tmb_xform_unpoint(xf, r.origin);
    tmb_vec2 d = tmb_v2(xf.c * r.dir.x + xf.s * r.dir.y,
                       -xf.s * r.dir.x + xf.c * r.dir.y);
    float lower = 0.0f, upper = (r.max_t > 0.0f) ? r.max_t : FLT_MAX;
    int index = -1;
    for (int i = 0; i < s->count; i++) {
        float num = tmb_v2_dot(s->normals[i], tmb_v2_sub(s->verts[i], o));
        float den = tmb_v2_dot(s->normals[i], d);
        if (den == 0.0f) {
            if (num < 0.0f)
                return 0;
        } else {
            if (den < 0.0f && num < lower * den) {
                lower = num / den;
                index = i;
            } else if (den > 0.0f && num < upper * den) {
                upper = num / den;
            }
        }
        if (upper < lower)
            return 0;
    }
    if (index < 0)
        return 0;
    *t = lower;
    if (normal)
        *normal = tmb_xform_dir(xf, s->normals[index]);
    return 1;
}

tmb_rayhit tmb_world_raycast(tmb_world *w, tmb_ray r)
{
    tmb_rayhit best;
    best.hit = 0;
    best.t = FLT_MAX;
    best.body = UINT32_MAX;
    best.point = tmb_v2(0, 0);
    best.normal = tmb_v2(0, 0);

    for (size_t i = 0; i < tmb_world_body_count(w); i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        float t;
        tmb_vec2 n;
        int hit = 0;
        if (b->shape.kind == TMB_SHAPE_CIRCLE)
            hit = tmb_ray_circle(r, b->pos, b->shape.radius, &t, &n);
        else
            hit = tmb_ray_polygon(r, &b->shape, b->xf, &t, &n);
        if (hit && t < best.t) {
            best.hit = 1;
            best.t = t;
            best.body = (uint32_t)i;
            best.normal = n;
            best.point = tmb_v2_add(r.origin, tmb_v2_scale(r.dir, t));
        }
    }
    return best;
}
