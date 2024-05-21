#ifndef TUMBLE_RAYCAST_H
#define TUMBLE_RAYCAST_H

#include "tumble/world.h"
#include "tumble/shape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    tmb_vec2 origin;
    tmb_vec2 dir; /* need not be unit length */
    float max_t;  /* parametric limit; <= 0 means unbounded */
} tmb_ray;

typedef struct {
    int hit;
    float t;
    uint32_t body;
    tmb_vec2 point;
    tmb_vec2 normal;
} tmb_rayhit;

int tmb_ray_circle(tmb_ray r, tmb_vec2 center, float radius, float *t,
                   tmb_vec2 *normal);
int tmb_ray_polygon(tmb_ray r, const tmb_shape *s, tmb_xform xf, float *t,
                    tmb_vec2 *normal);

/* Nearest body hit by the ray, if any. */
tmb_rayhit tmb_world_raycast(tmb_world *w, tmb_ray r);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_RAYCAST_H */
