#ifndef TUMBLE_RECORDER_H
#define TUMBLE_RECORDER_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Records per-body kinematic state each frame so a simulation can be replayed
 * or diffed deterministically. Frames are stored flat; each frame holds one
 * tmb_rec_state per body. Useful for regression tests ("does this scene evolve
 * the same way after a refactor?") and for scrubbing in a viewer. */

typedef struct {
    tmb_vec2 pos;
    float angle;
    tmb_vec2 vel;
    float ang_vel;
} tmb_rec_state;

typedef struct {
    tmb_rec_state *frames; /* frame_count * bodies_per_frame */
    size_t frame_count, frame_cap;
    size_t bodies_per_frame;
} tmb_recorder;

void tmb_recorder_init(tmb_recorder *r, size_t bodies_per_frame);
void tmb_recorder_free(tmb_recorder *r);

/* Capture the current world state as a new frame. Returns 0 on success. */
int tmb_recorder_capture(tmb_recorder *r, tmb_world *w);
/* Restore frame `index` into the world (positions, angles, velocities). */
int tmb_recorder_restore(const tmb_recorder *r, size_t index, tmb_world *w);
const tmb_rec_state *tmb_recorder_frame(const tmb_recorder *r, size_t index);

/* Largest per-body position delta between two frames; for divergence tests. */
float tmb_recorder_max_drift(const tmb_recorder *r, size_t f0, size_t f1);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_RECORDER_H */
