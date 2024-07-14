#include "tumble/forcefield.h"

tmb_field tmb_field_attractor(tmb_vec2 center, float radius, float strength)
{
    tmb_field f;
    f.kind = TMB_FIELD_RADIAL;
    f.falloff = TMB_FALLOFF_LINEAR;
    f.center = center;
    f.direction = tmb_v2(0, 0);
    f.radius = radius;
    f.strength = strength;
    return f;
}

tmb_field tmb_field_vortex(tmb_vec2 center, float radius, float strength)
{
    tmb_field f = tmb_field_attractor(center, radius, strength);
    f.kind = TMB_FIELD_VORTEX;
    return f;
}

tmb_field tmb_field_uniform(tmb_vec2 center, float radius, tmb_vec2 dir,
                            float strength)
{
    tmb_field f = tmb_field_attractor(center, radius, strength);
    f.kind = TMB_FIELD_UNIFORM;
    f.falloff = TMB_FALLOFF_NONE;
    f.direction = tmb_v2_normalize(dir);
    return f;
}

float tmb_field_weight(const tmb_field *f, float dist)
{
    if (f->radius <= 0)
        return 0;
    if (dist > f->radius)
        return 0;
    switch (f->falloff) {
    case TMB_FALLOFF_LINEAR:
        return 1.0f - dist / f->radius;
    case TMB_FALLOFF_INVERSE: {
        float d = dist < 0.1f ? 0.1f : dist;
        return 1.0f / (d * d);
    }
    case TMB_FALLOFF_NONE:
    default:
        return 1.0f;
    }
}

tmb_vec2 tmb_field_force_at(const tmb_field *f, tmb_vec2 p)
{
    tmb_vec2 to_center = tmb_v2_sub(f->center, p);
    float dist = tmb_v2_len(to_center);
    float w = tmb_field_weight(f, dist);
    if (w <= 0)
        return tmb_v2(0, 0);
    if (f->kind == TMB_FIELD_UNIFORM)
        return tmb_v2_scale(f->direction, f->strength * w);
    if (dist < 1e-5f)
        return tmb_v2(0, 0);
    tmb_vec2 dir = tmb_v2_scale(to_center, 1.0f / dist);
    if (f->kind == TMB_FIELD_VORTEX)
        dir = tmb_v2_perp(dir); /* tangential */
    return tmb_v2_scale(dir, f->strength * w);
}

size_t tmb_field_apply(const tmb_field *f, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    size_t affected = 0;
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC)
            continue;
        tmb_vec2 force = tmb_field_force_at(f, b->pos);
        if (force.x == 0 && force.y == 0)
            continue;
        /* scale by mass so heavier bodies feel a proportional acceleration */
        b->force = tmb_v2_add(b->force, tmb_v2_scale(force, b->mass));
        affected++;
    }
    return affected;
}
