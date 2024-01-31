#include "tumble/vec2.h"

#include <math.h>

float tmb_v2_len_sq(tmb_vec2 a) { return a.x * a.x + a.y * a.y; }
float tmb_v2_len(tmb_vec2 a) { return sqrtf(tmb_v2_len_sq(a)); }
float tmb_v2_dist(tmb_vec2 a, tmb_vec2 b) { return tmb_v2_len(tmb_v2_sub(a, b)); }

tmb_vec2 tmb_v2_normalize(tmb_vec2 a)
{
    float len = tmb_v2_len(a);
    if (len <= 1e-20f)
        return tmb_v2(0.0f, 0.0f);
    float inv = 1.0f / len;
    return tmb_v2(a.x * inv, a.y * inv);
}

tmb_vec2 tmb_v2_rotate(tmb_vec2 a, float radians)
{
    float c = cosf(radians), s = sinf(radians);
    return tmb_v2(a.x * c - a.y * s, a.x * s + a.y * c);
}
