#ifndef TUMBLE_SIM_H
#define TUMBLE_SIM_H

#include "tumble/world.h"
#include "tumble/island.h"
#include "tumble/timestep.h"
#include "tumble/profile.h"
#include "tumble/recorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A high-level driver that wires the optional subsystems around a world:
 * fixed-timestepping, island sleeping, optional recording and per-step
 * profiling. It owns nothing the caller did not ask for -- features are turned
 * on through the options and the world itself is supplied by the caller. */

typedef struct {
    int enable_sleeping;
    int enable_recording;
    int enable_profiling;
    float fixed_dt;
    size_t record_bodies; /* bodies per recorded frame (0 = body_count) */
} tmb_sim_opts;

typedef struct {
    tmb_world *world; /* not owned */
    tmb_islands islands;
    tmb_timestep ts;
    tmb_profile profile;
    tmb_recorder recorder;
    tmb_sim_opts opts;
    uint64_t steps;
    size_t sleeping;
} tmb_sim;

tmb_sim_opts tmb_sim_defaults(void);
void tmb_sim_init(tmb_sim *s, tmb_world *w, tmb_sim_opts opts);
void tmb_sim_free(tmb_sim *s);
/* Advance real time by frame_dt, running fixed steps and the enabled
 * subsystems. Returns the number of fixed steps run. */
int tmb_sim_advance(tmb_sim *s, float frame_dt);
/* Run exactly `count` fixed steps regardless of timing. */
void tmb_sim_run(tmb_sim *s, int count);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SIM_H */
