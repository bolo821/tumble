#include "tumble/buoyancy.h"
#include "tumble/transform.h"

#include <math.h>

tmb_fluid tmb_fluid_make(float surface_y, float density, tmb_vec2 gravity)
{
    tmb_fluid f;
    f.surface_y = surface_y;
    f.density = density;
    f.gravity = gravity;
    f.linear_drag = 1.5f;
    f.current = tmb_v2(0, 0);
    return f;
}

float tmb_fluid_submersion(const tmb_fluid *f, tmb_body *b)
{
    tmb_body_sync(b);
    tmb_aabb box = tmb_shape_aabb(&b->shape, b->xf);
    float height = box.max.y - box.min.y;
    if (height < 1e-6f)
        return 0.0f;
    if (box.max.y <= f->surface_y)
        return 1.0f; /* fully under */
    if (box.min.y >= f->surface_y)
        return 0.0f; /* fully above */
    float submerged = f->surface_y - box.min.y;
    float frac = submerged / height;
    return frac < 0 ? 0 : (frac > 1 ? 1 : frac);
}

size_t tmb_fluid_apply(const tmb_fluid *f, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    size_t wet = 0;
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC || b->mass <= 0)
            continue;
        float frac = tmb_fluid_submersion(f, b);
        if (frac <= 0)
            continue;
        wet++;
        /* displaced "area" ~ body mass/density scaled by submerged fraction;
         * buoyant force opposes gravity */
        float volume = b->mass / (b->density > 0 ? b->density : 1.0f);
        float displaced = volume * frac;
        tmb_vec2 buoyant = tmb_v2_scale(f->gravity, -f->density * displaced);
        b->force = tmb_v2_add(b->force, buoyant);
        /* drag relative to the current */
        tmb_vec2 rel = tmb_v2_sub(b->vel, f->current);
        tmb_vec2 drag = tmb_v2_scale(rel, -f->linear_drag * frac * b->mass);
        b->force = tmb_v2_add(b->force, drag);
        b->torque -= f->linear_drag * frac * b->inertia * b->ang_vel;
    }
    return wet;
}
