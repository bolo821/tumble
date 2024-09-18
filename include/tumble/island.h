#ifndef TUMBLE_ISLAND_H
#define TUMBLE_ISLAND_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Island detection: group dynamic bodies that are connected (directly or
 * transitively) through contacts or joints, using union-find. Islands are the
 * unit at which sleeping is decided -- an island sleeps only when every body in
 * it has been below the motion thresholds long enough. */

typedef struct {
    uint32_t *parent;   /* union-find forest over body indices */
    uint32_t *rank;
    float *sleep_timer; /* per body, seconds below threshold */
    uint8_t *sleeping;  /* per body flag */
    size_t count;
    size_t cap;
    float linear_tol;   /* squared linear speed below which a body is "still" */
    float angular_tol;  /* angular speed below which a body is "still" */
    float time_to_sleep;
} tmb_islands;

void tmb_islands_init(tmb_islands *is);
void tmb_islands_free(tmb_islands *is);

/* Ensure capacity for the world's bodies and reset the union-find forest. */
int tmb_islands_begin(tmb_islands *is, tmb_world *w);
/* Merge the islands containing bodies i and j. */
void tmb_islands_link(tmb_islands *is, uint32_t i, uint32_t j);
/* Representative root of body i (with path compression). */
uint32_t tmb_islands_find(tmb_islands *is, uint32_t i);
/* Build links from every contact and joint in the world. */
void tmb_islands_build(tmb_islands *is, tmb_world *w);
/* Count distinct islands among dynamic bodies. */
size_t tmb_islands_count(tmb_islands *is, tmb_world *w);

/* Advance sleep timers by dt and put fully-still islands to sleep. Returns the
 * number of bodies currently sleeping. */
size_t tmb_islands_update_sleep(tmb_islands *is, tmb_world *w, float dt);
int tmb_islands_is_sleeping(const tmb_islands *is, uint32_t i);
void tmb_islands_wake(tmb_islands *is, uint32_t i);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_ISLAND_H */
