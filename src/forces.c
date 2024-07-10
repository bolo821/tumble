#include "tumble/forces.h"

#include <math.h>

void tmb_body_apply_force(tmb_body *b, tmb_vec2 force, tmb_vec2 point)
{
    b->force = tmb_v2_add(b->force, force);
    b->torque += tmb_v2_cross(tmb_v2_sub(point, b->pos), force);
}

void tmb_body_apply_force_center(tmb_body *b, tmb_vec2 force)
{
    b->force = tmb_v2_add(b->force, force);
}

void tmb_body_apply_torque(tmb_body *b, float torque) { b->torque += torque; }

void tmb_body_apply_impulse(tmb_body *b, tmb_vec2 impulse, tmb_vec2 point)
{
    if (b->type != TMB_BODY_DYNAMIC)
        return;
    b->vel = tmb_v2_add(b->vel, tmb_v2_scale(impulse, b->inv_mass));
    b->ang_vel += b->inv_inertia * tmb_v2_cross(tmb_v2_sub(point, b->pos), impulse);
}

void tmb_apply_uniform(tmb_world *w, tmb_vec2 accel)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type == TMB_BODY_DYNAMIC && b->mass > 0.0f)
            b->force = tmb_v2_add(b->force, tmb_v2_scale(accel, b->mass));
    }
}

void tmb_apply_drag(tmb_world *w, float linear, float angular)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        b->force = tmb_v2_sub(b->force, tmb_v2_scale(b->vel, linear));
        b->torque -= angular * b->ang_vel;
    }
}

void tmb_apply_wind(tmb_world *w, tmb_vec2 wind, float coefficient)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        tmb_vec2 rel = tmb_v2_sub(wind, b->vel);
        b->force = tmb_v2_add(b->force, tmb_v2_scale(rel, coefficient));
    }
}

void tmb_apply_attractor(tmb_world *w, tmb_vec2 center, float strength,
                         float falloff)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC || b->mass <= 0.0f)
            continue;
        tmb_vec2 d = tmb_v2_sub(center, b->pos);
        float dist = tmb_v2_len(d);
        if (dist < 1e-4f)
            continue;
        float mag = strength * b->mass / (1.0f + falloff * dist * dist);
        b->force = tmb_v2_add(b->force, tmb_v2_scale(tmb_v2_scale(d, 1.0f / dist), mag));
    }
}

void tmb_apply_explosion(tmb_world *w, tmb_vec2 center, float strength,
                         float radius)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        tmb_vec2 d = tmb_v2_sub(b->pos, center);
        float dist = tmb_v2_len(d);
        if (dist > radius || dist < 1e-4f)
            continue;
        float falloff = 1.0f - dist / radius;
        tmb_vec2 dir = tmb_v2_scale(d, 1.0f / dist);
        tmb_body_apply_impulse(b, tmb_v2_scale(dir, strength * falloff), b->pos);
    }
}
