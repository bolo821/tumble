#include "tumble/wind.h"

void tmb_wind_init(tmb_wind *wind, uint32_t seed, float strength)
{
    tmb_noise_init(&wind->noise, seed);
    wind->scale = 0.15f;
    wind->strength = strength;
    wind->drag = 0.8f;
    wind->time = 0.0f;
}

tmb_vec2 tmb_wind_sample(const tmb_wind *wind, tmb_vec2 p)
{
    tmb_vec2 flow = tmb_noise_flow(&wind->noise, p.x * wind->scale + wind->time,
                                   p.y * wind->scale);
    return tmb_v2_scale(flow, wind->strength);
}

size_t tmb_wind_apply(tmb_wind *wind, tmb_world *w, float dt)
{
    size_t n = tmb_world_body_count(w);
    size_t affected = 0;
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC || b->mass <= 0)
            continue;
        tmb_vec2 flow = tmb_wind_sample(wind, b->pos);
        tmb_vec2 rel = tmb_v2_sub(flow, b->vel);
        tmb_vec2 force = tmb_v2_scale(rel, wind->drag * b->mass);
        b->force = tmb_v2_add(b->force, force);
        affected++;
    }
    wind->time += dt * 0.5f;
    return affected;
}
