#ifndef TUMBLE_SPAWNER_H
#define TUMBLE_SPAWNER_H

#include "tumble/world.h"
#include "tumble/rng.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A timed emitter that drops bodies into the world at a fixed rate, with
 * randomized size and initial velocity. Drives fountains, rain and particle
 * showers. Call tmb_spawner_update once per step; it spawns as many bodies as
 * the elapsed time allows, up to a lifetime budget. */

typedef enum {
    TMB_SPAWN_CIRCLE,
    TMB_SPAWN_BOX
} tmb_spawn_shape;

typedef struct {
    tmb_vec2 origin;
    tmb_vec2 spread;      /* +/- jitter on the spawn position */
    tmb_vec2 velocity;    /* base launch velocity */
    tmb_vec2 vel_jitter;  /* +/- jitter on velocity */
    float rate;           /* bodies per second */
    float min_size, max_size;
    tmb_spawn_shape shape;
    int max_total;        /* stop after this many (<=0 = unlimited) */

    /* state */
    tmb_rng rng;
    float accum;
    int total;
} tmb_spawner;

void tmb_spawner_init(tmb_spawner *s, tmb_vec2 origin, float rate,
                      uint32_t seed);
/* Advance by dt and emit bodies; returns the number spawned this call. */
int tmb_spawner_update(tmb_spawner *s, tmb_world *w, float dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SPAWNER_H */
