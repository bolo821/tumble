#include "tumble/motion.h"
#include "tumble/shape.h"

float tmb_world_total_mass(const tmb_world *w)
{
    float m = 0.0f;
    for (size_t i = 0; i < w->body_count; i++)
        m += w->bodies[i].mass;
    return m;
}

float tmb_world_kinetic_energy(const tmb_world *w)
{
    float e = 0.0f;
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        e += 0.5f * b->mass * tmb_v2_len_sq(b->vel);
        e += 0.5f * b->inertia * b->ang_vel * b->ang_vel;
    }
    return e;
}

tmb_vec2 tmb_world_linear_momentum(const tmb_world *w)
{
    tmb_vec2 p = tmb_v2(0, 0);
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        if (b->type == TMB_BODY_DYNAMIC)
            p = tmb_v2_add(p, tmb_v2_scale(b->vel, b->mass));
    }
    return p;
}

float tmb_world_angular_momentum(const tmb_world *w)
{
    float l = 0.0f;
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        l += b->inertia * b->ang_vel + b->mass * tmb_v2_cross(b->pos, b->vel);
    }
    return l;
}

tmb_vec2 tmb_world_center_of_mass(const tmb_world *w)
{
    tmb_vec2 c = tmb_v2(0, 0);
    float total = 0.0f;
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        c = tmb_v2_add(c, tmb_v2_scale(b->pos, b->mass));
        total += b->mass;
    }
    return total > 0.0f ? tmb_v2_scale(c, 1.0f / total) : c;
}

tmb_aabb tmb_world_bounds(tmb_world *w)
{
    tmb_aabb box = tmb_aabb_empty();
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        tmb_aabb bb = tmb_shape_aabb(&b->shape, b->xf);
        box = tmb_aabb_combine(box, bb);
    }
    return box;
}
