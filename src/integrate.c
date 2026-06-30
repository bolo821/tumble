#include "tumble/integrate.h"
#include "tumble/island_sleep.h"

void tmb_integrate_forces(tmb_world *w, float dt)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC || b->inv_mass == 0.0f)
            continue;
        if (tmb_body_is_sleeping(w, (uint32_t)i))
            continue; /* frozen bodies don't accumulate impulses */
        /* v += (g + F/m) dt */
        tmb_vec2 acc = tmb_v2_add(w->gravity, tmb_v2_scale(b->force, b->inv_mass));
        b->vel = tmb_v2_add(b->vel, tmb_v2_scale(acc, dt));
        b->ang_vel += b->torque * b->inv_inertia * dt;
    }
}

void tmb_integrate_velocities(tmb_world *w, float dt)
{
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC)
            continue;
        if (tmb_body_is_sleeping(w, (uint32_t)i))
            continue; /* frozen bodies don't move */
        b->pos = tmb_v2_add(b->pos, tmb_v2_scale(b->vel, dt));
        b->angle += b->ang_vel * dt;
        tmb_body_sync(b);
    }
}
