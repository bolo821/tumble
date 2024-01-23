#ifndef TUMBLE_AABB_H
#define TUMBLE_AABB_H

#include "tumble/vec2.h"
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An axis-aligned bounding box. An empty box has min > max. */
typedef struct {
    tmb_vec2 min;
    tmb_vec2 max;
} tmb_aabb;

static inline tmb_aabb tmb_aabb_empty(void)
{
    tmb_aabb b = {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}};
    return b;
}
static inline void tmb_aabb_grow(tmb_aabb *b, tmb_vec2 p)
{
    b->min = tmb_v2_min(b->min, p);
    b->max = tmb_v2_max(b->max, p);
}
static inline int tmb_aabb_overlap(tmb_aabb a, tmb_aabb b)
{
    return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y &&
           a.max.y >= b.min.y;
}
static inline int tmb_aabb_contains(tmb_aabb b, tmb_vec2 p)
{
    return p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y && p.y <= b.max.y;
}
static inline tmb_aabb tmb_aabb_combine(tmb_aabb a, tmb_aabb b)
{
    tmb_aabb r;
    r.min = tmb_v2_min(a.min, b.min);
    r.max = tmb_v2_max(a.max, b.max);
    return r;
}
static inline int tmb_aabb_contains_box(tmb_aabb outer, tmb_aabb inner)
{
    return outer.min.x <= inner.min.x && outer.min.y <= inner.min.y &&
           inner.max.x <= outer.max.x && inner.max.y <= outer.max.y;
}
static inline tmb_vec2 tmb_aabb_center(tmb_aabb b)
{
    return tmb_v2_scale(tmb_v2_add(b.min, b.max), 0.5f);
}
static inline float tmb_aabb_perimeter(tmb_aabb b)
{
    return 2.0f * ((b.max.x - b.min.x) + (b.max.y - b.min.y));
}

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_AABB_H */
