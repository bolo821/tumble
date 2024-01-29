#ifndef TUMBLE_VEC2_H
#define TUMBLE_VEC2_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The 2D vector type and the inline math the whole engine is built on. */
typedef struct {
    float x, y;
} tmb_vec2;

static inline tmb_vec2 tmb_v2(float x, float y)
{
    tmb_vec2 v = {x, y};
    return v;
}
static inline tmb_vec2 tmb_v2_add(tmb_vec2 a, tmb_vec2 b)
{
    return tmb_v2(a.x + b.x, a.y + b.y);
}
static inline tmb_vec2 tmb_v2_sub(tmb_vec2 a, tmb_vec2 b)
{
    return tmb_v2(a.x - b.x, a.y - b.y);
}
static inline tmb_vec2 tmb_v2_scale(tmb_vec2 a, float s)
{
    return tmb_v2(a.x * s, a.y * s);
}
static inline float tmb_v2_dot(tmb_vec2 a, tmb_vec2 b)
{
    return a.x * b.x + a.y * b.y;
}
/* The scalar (z component of the) 2D cross product. */
static inline float tmb_v2_cross(tmb_vec2 a, tmb_vec2 b)
{
    return a.x * b.y - a.y * b.x;
}
/* Cross of a vector with a scalar, and scalar with a vector (a x s, s x a). */
static inline tmb_vec2 tmb_v2_cross_vs(tmb_vec2 a, float s)
{
    return tmb_v2(s * a.y, -s * a.x);
}
static inline tmb_vec2 tmb_v2_cross_sv(float s, tmb_vec2 a)
{
    return tmb_v2(-s * a.y, s * a.x);
}
/* Left perpendicular. */
static inline tmb_vec2 tmb_v2_perp(tmb_vec2 a)
{
    return tmb_v2(-a.y, a.x);
}
static inline tmb_vec2 tmb_v2_lerp(tmb_vec2 a, tmb_vec2 b, float t)
{
    return tmb_v2_add(a, tmb_v2_scale(tmb_v2_sub(b, a), t));
}
static inline tmb_vec2 tmb_v2_min(tmb_vec2 a, tmb_vec2 b)
{
    return tmb_v2(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y);
}
static inline tmb_vec2 tmb_v2_max(tmb_vec2 a, tmb_vec2 b)
{
    return tmb_v2(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y);
}
static inline tmb_vec2 tmb_v2_neg(tmb_vec2 a) { return tmb_v2(-a.x, -a.y); }

/* Out-of-line (use <math.h>). */
float tmb_v2_len(tmb_vec2 a);
float tmb_v2_len_sq(tmb_vec2 a);
float tmb_v2_dist(tmb_vec2 a, tmb_vec2 b);
tmb_vec2 tmb_v2_normalize(tmb_vec2 a);
tmb_vec2 tmb_v2_rotate(tmb_vec2 a, float radians);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_VEC2_H */
