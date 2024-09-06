#include "tumble/spawner.h"

void tmb_spawner_init(tmb_spawner *s, tmb_vec2 origin, float rate,
                      uint32_t seed)
{
    s->origin = origin;
    s->spread = tmb_v2(0.5f, 0.0f);
    s->velocity = tmb_v2(0.0f, 5.0f);
    s->vel_jitter = tmb_v2(2.0f, 1.0f);
    s->rate = rate > 0 ? rate : 1.0f;
    s->min_size = 0.2f;
    s->max_size = 0.4f;
    s->shape = TMB_SPAWN_CIRCLE;
    s->max_total = 0;
    tmb_rng_seed(&s->rng, seed);
    s->accum = 0;
    s->total = 0;
}

static float jitter(tmb_rng *r, float base, float amp)
{
    return base + tmb_rng_range(r, -amp, amp);
}

int tmb_spawner_update(tmb_spawner *s, tmb_world *w, float dt)
{
    if (dt <= 0)
        return 0;
    s->accum += dt * s->rate;
    int spawned = 0;
    while (s->accum >= 1.0f) {
        s->accum -= 1.0f;
        if (s->max_total > 0 && s->total >= s->max_total)
            break;
        tmb_vec2 pos = tmb_v2(jitter(&s->rng, s->origin.x, s->spread.x),
                              jitter(&s->rng, s->origin.y, s->spread.y));
        float size = tmb_rng_range(&s->rng, s->min_size, s->max_size);
        tmb_shape shape = s->shape == TMB_SPAWN_BOX
                              ? tmb_shape_box(size, size)
                              : tmb_shape_circle(size);
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, shape, pos);
        b.vel = tmb_v2(jitter(&s->rng, s->velocity.x, s->vel_jitter.x),
                       jitter(&s->rng, s->velocity.y, s->vel_jitter.y));
        b.restitution = 0.3f;
        if (tmb_world_add_body(w, &b) == UINT32_MAX)
            break;
        s->total++;
        spawned++;
    }
    return spawned;
}
