#include "tumble/shape.h"

#include <math.h>
#include <string.h>

#ifndef TMB_PI
#define TMB_PI 3.14159265358979323846f
#endif

tmb_shape tmb_shape_circle(float radius)
{
    tmb_shape s;
    memset(&s, 0, sizeof(s));
    s.kind = TMB_SHAPE_CIRCLE;
    s.radius = radius;
    return s;
}

static void compute_normals(tmb_shape *s)
{
    for (int i = 0; i < s->count; i++) {
        tmb_vec2 e = tmb_v2_sub(s->verts[(i + 1) % s->count], s->verts[i]);
        /* outward normal for CCW winding is the right perpendicular */
        s->normals[i] = tmb_v2_normalize(tmb_v2(e.y, -e.x));
    }
}

tmb_err tmb_shape_polygon(tmb_shape *out, const tmb_vec2 *verts, int n)
{
    if (n < 3 || n > TMB_MAX_POLY_VERTS)
        return TMB_ERR_DEGENERATE;
    memset(out, 0, sizeof(*out));
    out->kind = TMB_SHAPE_POLYGON;
    out->count = n;
    for (int i = 0; i < n; i++)
        out->verts[i] = verts[i];

    /* signed area; if clockwise, reverse to CCW */
    float area2 = 0.0f;
    for (int i = 0; i < n; i++)
        area2 += tmb_v2_cross(out->verts[i], out->verts[(i + 1) % n]);
    if (fabsf(area2) < 1e-12f)
        return TMB_ERR_DEGENERATE;
    if (area2 < 0.0f) {
        for (int i = 0; i < n / 2; i++) {
            tmb_vec2 t = out->verts[i];
            out->verts[i] = out->verts[n - 1 - i];
            out->verts[n - 1 - i] = t;
        }
    }
    compute_normals(out);
    return TMB_OK;
}

tmb_shape tmb_shape_box(float hw, float hh)
{
    tmb_shape s;
    tmb_vec2 v[4] = {{-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};
    tmb_shape_polygon(&s, v, 4);
    return s;
}

tmb_err tmb_shape_regular(tmb_shape *out, int sides, float radius)
{
    if (sides < 3 || sides > TMB_MAX_POLY_VERTS)
        return TMB_ERR_DEGENERATE;
    tmb_vec2 v[TMB_MAX_POLY_VERTS];
    for (int i = 0; i < sides; i++) {
        float a = 2.0f * TMB_PI * (float)i / (float)sides;
        v[i] = tmb_v2(cosf(a) * radius, sinf(a) * radius);
    }
    return tmb_shape_polygon(out, v, sides);
}

tmb_aabb tmb_shape_aabb(const tmb_shape *s, tmb_xform xf)
{
    tmb_aabb b = tmb_aabb_empty();
    if (s->kind == TMB_SHAPE_CIRCLE) {
        tmb_vec2 c = xf.pos;
        b.min = tmb_v2(c.x - s->radius, c.y - s->radius);
        b.max = tmb_v2(c.x + s->radius, c.y + s->radius);
        return b;
    }
    for (int i = 0; i < s->count; i++)
        tmb_aabb_grow(&b, tmb_xform_point(xf, s->verts[i]));
    return b;
}

void tmb_shape_compute_mass(const tmb_shape *s, float density, float *mass,
                            float *inertia, tmb_vec2 *centroid)
{
    if (s->kind == TMB_SHAPE_CIRCLE) {
        float m = density * TMB_PI * s->radius * s->radius;
        *mass = m;
        *inertia = 0.5f * m * s->radius * s->radius;
        *centroid = tmb_v2(0, 0);
        return;
    }
    /* polygon: accumulate area, centroid and second moment over a fan */
    tmb_vec2 c = tmb_v2(0, 0);
    float area = 0.0f;
    float I = 0.0f;
    tmb_vec2 ref = s->verts[0];
    const float inv3 = 1.0f / 3.0f;
    for (int i = 0; i < s->count; i++) {
        tmb_vec2 e1 = tmb_v2_sub(s->verts[i], ref);
        tmb_vec2 e2 = tmb_v2_sub(s->verts[(i + 1) % s->count], ref);
        float d = tmb_v2_cross(e1, e2);
        float triArea = 0.5f * d;
        area += triArea;
        c = tmb_v2_add(c, tmb_v2_scale(tmb_v2_add(e1, e2), triArea * inv3));
        float intx2 = e1.x * e1.x + e2.x * e1.x + e2.x * e2.x;
        float inty2 = e1.y * e1.y + e2.y * e1.y + e2.y * e2.y;
        I += (0.25f * inv3 * d) * (intx2 + inty2);
    }
    if (area < 1e-12f) {
        *mass = 0;
        *inertia = 0;
        *centroid = ref;
        return;
    }
    c = tmb_v2_scale(c, 1.0f / area);
    *mass = density * area;
    *centroid = tmb_v2_add(ref, c);
    /* shift inertia to the centroid (parallel axis), then scale by density */
    *inertia = density * I;
    *inertia -= (*mass) * tmb_v2_dot(c, c);
}

tmb_vec2 tmb_shape_support(const tmb_shape *s, tmb_vec2 d)
{
    if (s->kind == TMB_SHAPE_CIRCLE)
        return tmb_v2_scale(tmb_v2_normalize(d), s->radius);
    int best = 0;
    float bestDot = tmb_v2_dot(s->verts[0], d);
    for (int i = 1; i < s->count; i++) {
        float dot = tmb_v2_dot(s->verts[i], d);
        if (dot > bestDot) {
            bestDot = dot;
            best = i;
        }
    }
    return s->verts[best];
}
