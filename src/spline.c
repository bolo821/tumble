#include "tumble/spline.h"

tmb_vec2 tmb_catmull_rom(tmb_vec2 p0, tmb_vec2 p1, tmb_vec2 p2, tmb_vec2 p3,
                         float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    tmb_vec2 r;
    r.x = 0.5f * ((2 * p1.x) + (-p0.x + p2.x) * t +
                  (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 +
                  (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
    r.y = 0.5f * ((2 * p1.y) + (-p0.y + p2.y) * t +
                  (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 +
                  (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);
    return r;
}

tmb_vec2 tmb_catmull_tangent(tmb_vec2 p0, tmb_vec2 p1, tmb_vec2 p2, tmb_vec2 p3,
                             float t)
{
    float t2 = t * t;
    tmb_vec2 r;
    r.x = 0.5f * ((-p0.x + p2.x) +
                  2 * (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t +
                  3 * (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t2);
    r.y = 0.5f * ((-p0.y + p2.y) +
                  2 * (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t +
                  3 * (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t2);
    return r;
}

int tmb_spline_sample(const tmb_vec2 *ctrl, int n, tmb_vec2 *out, int samples)
{
    if (n < 2 || samples < 2)
        return 0;
    int written = 0;
    for (int i = 0; i < samples; i++) {
        float u = (float)i / (float)(samples - 1) * (float)(n - 1);
        int seg = (int)u;
        if (seg >= n - 1)
            seg = n - 2;
        float t = u - (float)seg;
        int i0 = seg - 1 < 0 ? 0 : seg - 1;
        int i1 = seg;
        int i2 = seg + 1;
        int i3 = seg + 2 > n - 1 ? n - 1 : seg + 2;
        out[written++] =
            tmb_catmull_rom(ctrl[i0], ctrl[i1], ctrl[i2], ctrl[i3], t);
    }
    return written;
}

tmb_vec2 tmb_bezier3(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d, float t)
{
    float u = 1.0f - t;
    float uu = u * u, tt = t * t;
    float w0 = uu * u, w1 = 3 * uu * t, w2 = 3 * u * tt, w3 = tt * t;
    tmb_vec2 r;
    r.x = w0 * a.x + w1 * b.x + w2 * c.x + w3 * d.x;
    r.y = w0 * a.y + w1 * b.y + w2 * c.y + w3 * d.y;
    return r;
}

float tmb_bezier3_length(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d,
                         int steps)
{
    if (steps < 1)
        steps = 1;
    float len = 0;
    tmb_vec2 prev = a;
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / (float)steps;
        tmb_vec2 p = tmb_bezier3(a, b, c, d, t);
        len += tmb_v2_dist(prev, p);
        prev = p;
    }
    return len;
}
