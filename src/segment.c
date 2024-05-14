#include "tumble/segment.h"

#include <math.h>

tmb_vec2 tmb_seg_closest(tmb_vec2 a, tmb_vec2 b, tmb_vec2 p, float *t)
{
    tmb_vec2 ab = tmb_v2_sub(b, a);
    float len2 = tmb_v2_dot(ab, ab);
    float u = 0.0f;
    if (len2 > 1e-12f)
        u = tmb_v2_dot(tmb_v2_sub(p, a), ab) / len2;
    if (u < 0)
        u = 0;
    else if (u > 1)
        u = 1;
    if (t)
        *t = u;
    return tmb_v2_add(a, tmb_v2_scale(ab, u));
}

float tmb_seg_point_dist(tmb_vec2 a, tmb_vec2 b, tmb_vec2 p)
{
    tmb_vec2 c = tmb_seg_closest(a, b, p, NULL);
    return tmb_v2_len(tmb_v2_sub(p, c));
}

int tmb_seg_intersect(tmb_segment s1, tmb_segment s2, tmb_vec2 *hit, float *t1,
                      float *t2)
{
    tmb_vec2 r = tmb_v2_sub(s1.b, s1.a);
    tmb_vec2 s = tmb_v2_sub(s2.b, s2.a);
    float rxs = tmb_v2_cross(r, s);
    if (rxs > -1e-12f && rxs < 1e-12f)
        return 0; /* parallel */
    tmb_vec2 qp = tmb_v2_sub(s2.a, s1.a);
    float t = tmb_v2_cross(qp, s) / rxs;
    float u = tmb_v2_cross(qp, r) / rxs;
    if (t < 0 || t > 1 || u < 0 || u > 1)
        return 0;
    if (t1)
        *t1 = t;
    if (t2)
        *t2 = u;
    if (hit)
        *hit = tmb_v2_add(s1.a, tmb_v2_scale(r, t));
    return 1;
}

float tmb_seg_seg_dist(tmb_segment s1, tmb_segment s2, tmb_vec2 *p1,
                       tmb_vec2 *p2)
{
    tmb_vec2 h;
    if (tmb_seg_intersect(s1, s2, &h, NULL, NULL)) {
        if (p1)
            *p1 = h;
        if (p2)
            *p2 = h;
        return 0.0f;
    }
    /* otherwise the minimum is at one of the four endpoint-to-segment cases */
    tmb_vec2 c[4];
    float d[4];
    c[0] = tmb_seg_closest(s2.a, s2.b, s1.a, NULL);
    d[0] = tmb_v2_len(tmb_v2_sub(s1.a, c[0]));
    c[1] = tmb_seg_closest(s2.a, s2.b, s1.b, NULL);
    d[1] = tmb_v2_len(tmb_v2_sub(s1.b, c[1]));
    c[2] = tmb_seg_closest(s1.a, s1.b, s2.a, NULL);
    d[2] = tmb_v2_len(tmb_v2_sub(s2.a, c[2]));
    c[3] = tmb_seg_closest(s1.a, s1.b, s2.b, NULL);
    d[3] = tmb_v2_len(tmb_v2_sub(s2.b, c[3]));
    int best = 0;
    for (int i = 1; i < 4; i++)
        if (d[i] < d[best])
            best = i;
    if (p1 && p2) {
        switch (best) {
        case 0:
            *p1 = s1.a;
            *p2 = c[0];
            break;
        case 1:
            *p1 = s1.b;
            *p2 = c[1];
            break;
        case 2:
            *p1 = c[2];
            *p2 = s2.a;
            break;
        default:
            *p1 = c[3];
            *p2 = s2.b;
            break;
        }
    }
    return d[best];
}

float tmb_seg_ray_circle(tmb_vec2 origin, tmb_vec2 dir, tmb_vec2 center,
                     float radius)
{
    tmb_vec2 m = tmb_v2_sub(origin, center);
    float b = tmb_v2_dot(m, dir);
    float c = tmb_v2_dot(m, m) - radius * radius;
    if (c > 0 && b > 0)
        return -1.0f;
    float disc = b * b - c;
    if (disc < 0)
        return -1.0f;
    float t = -b - sqrtf(disc);
    if (t < 0)
        t = 0;
    return t;
}
