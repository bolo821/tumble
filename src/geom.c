#include "tumble/geom.h"

#include <math.h>

float tmb_orient2d(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

int tmb_segments_cross(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d)
{
    float d1 = tmb_orient2d(c, d, a);
    float d2 = tmb_orient2d(c, d, b);
    float d3 = tmb_orient2d(a, b, c);
    float d4 = tmb_orient2d(a, b, d);
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
        return 1;
    return 0;
}

void tmb_barycentric(tmb_vec2 p, tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, float *u,
                     float *v, float *w)
{
    tmb_vec2 v0 = tmb_v2_sub(b, a);
    tmb_vec2 v1 = tmb_v2_sub(c, a);
    tmb_vec2 v2 = tmb_v2_sub(p, a);
    float d00 = tmb_v2_dot(v0, v0);
    float d01 = tmb_v2_dot(v0, v1);
    float d11 = tmb_v2_dot(v1, v1);
    float d20 = tmb_v2_dot(v2, v0);
    float d21 = tmb_v2_dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    if (denom > -1e-12f && denom < 1e-12f) {
        *u = 1;
        *v = 0;
        *w = 0;
        return;
    }
    float inv = 1.0f / denom;
    float vv = (d11 * d20 - d01 * d21) * inv;
    float ww = (d00 * d21 - d01 * d20) * inv;
    *v = vv;
    *w = ww;
    *u = 1.0f - vv - ww;
}

int tmb_circumcircle(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 *center,
                     float *radius)
{
    float d = 2.0f * (a.x * (b.y - c.y) + b.x * (c.y - a.y) +
                      c.x * (a.y - b.y));
    if (d > -1e-12f && d < 1e-12f)
        return -1;
    float a2 = a.x * a.x + a.y * a.y;
    float b2 = b.x * b.x + b.y * b.y;
    float c2 = c.x * c.x + c.y * c.y;
    tmb_vec2 ctr;
    ctr.x = (a2 * (b.y - c.y) + b2 * (c.y - a.y) + c2 * (a.y - b.y)) / d;
    ctr.y = (a2 * (c.x - b.x) + b2 * (a.x - c.x) + c2 * (b.x - a.x)) / d;
    if (center)
        *center = ctr;
    if (radius)
        *radius = tmb_v2_dist(ctr, a);
    return 0;
}

tmb_vec2 tmb_project_point_line(tmb_vec2 p, tmb_vec2 a, tmb_vec2 dir)
{
    float len2 = tmb_v2_dot(dir, dir);
    if (len2 < 1e-12f)
        return a;
    float t = tmb_v2_dot(tmb_v2_sub(p, a), dir) / len2;
    return tmb_v2_add(a, tmb_v2_scale(dir, t));
}

tmb_vec2 tmb_reflect(tmb_vec2 v, tmb_vec2 n)
{
    float d = tmb_v2_dot(v, n);
    return tmb_v2_sub(v, tmb_v2_scale(n, 2.0f * d));
}

float tmb_angle_between(tmb_vec2 a, tmb_vec2 b)
{
    return atan2f(tmb_v2_cross(a, b), tmb_v2_dot(a, b));
}
