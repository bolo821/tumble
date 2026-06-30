#ifndef TUMBLE_ISLAND_SLEEP_H
#define TUMBLE_ISLAND_SLEEP_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Persistent island sleeping.
 *
 * Bodies that stop moving waste solver time, so once every dynamic body in a
 * connected island (bodies linked through contacts or joints) has been still
 * long enough, the island is put to sleep: its bodies are frozen and skipped
 * by the integrator until something disturbs them.
 *
 * Unlike the per-step island pass in island.c, this state persists across
 * steps. When an island sleeps it is recorded once in a heap-allocated
 * `tmb_island_rec`; every member body points at that shared record so a later
 * wake can reactivate the whole island in one shot without rescanning. The
 * world owns one `tmb_sleep_system`, created lazily when `enable_sleeping` is
 * set. */

typedef struct tmb_island_rec {
    uint32_t *members; /* body indices belonging to this sleeping island */
    int count;
    int cap;
} tmb_island_rec;

typedef struct tmb_sleep_system {
    tmb_island_rec **body_rec; /* body index -> its island record (or NULL) */
    float *still;              /* per body: seconds spent below the threshold */
    uint8_t *asleep;           /* per body: 1 if frozen */
    uint32_t *uf;              /* union-find scratch, rebuilt each update */
    size_t cap;                /* capacity of the per-body arrays */

    tmb_island_rec **owned;    /* every live record, for clean teardown */
    size_t owned_count, owned_cap;

    float linear_tol;          /* squared linear speed considered "still" */
    float angular_tol;         /* angular speed considered "still" */
    float time_to_sleep;       /* seconds an island must stay still */
    size_t sleeping_count;     /* bodies currently asleep */
} tmb_sleep_system;

tmb_sleep_system *tmb_sleep_create(void);
void tmb_sleep_destroy(tmb_sleep_system *s);

/* Grow the per-body arrays to cover the world's body capacity. */
int tmb_sleep_ensure(tmb_world *w);

/* Run one sleep/wake update for the step that just completed. */
void tmb_sleep_update(tmb_world *w, float dt);

/* Bookkeeping hook invoked by tmb_world_remove_body before the swap-pop: body
 * `i` is being removed and `last` is the body that will be moved into its
 * slot. */
void tmb_sleep_on_remove(tmb_world *w, uint32_t i, uint32_t last);

int tmb_body_is_sleeping(const tmb_world *w, uint32_t i);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_ISLAND_SLEEP_H */
