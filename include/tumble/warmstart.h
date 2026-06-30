#ifndef TUMBLE_WARMSTART_H
#define TUMBLE_WARMSTART_H

#include "tumble/contact.h"
#include "tumble/world.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Per-pair warm-start slot. prev points into the slab below. */
typedef struct {
    uint64_t pair;       /* packed (lo<<32|hi) body pair; 0 = empty */
    tmb_contact *prev;   /* pointer into slab — valid only while slab unmodified */
} tmb_ws_slot;

/* Contact impulse warm-start cache.  Speeds convergence by carrying the last
 * frame's accumulated impulses forward as the starting estimate for this frame.
 *
 * Lifecycle: call tmb_ws_store() at the end of each step after solving, then
 * tmb_ws_apply() at the start of the next step after the narrowphase has rebuilt
 * the contact set. */
typedef struct {
    tmb_ws_slot *table;   /* open-address hash table (power-of-2 capacity) */
    size_t table_cap;
    size_t table_count;

    tmb_contact *slab;    /* contiguous block of saved tmb_contact copies */
    size_t slab_count;
    size_t slab_cap;
} tmb_ws_cache;

void tmb_ws_cache_init(tmb_ws_cache *c);
void tmb_ws_cache_free(tmb_ws_cache *c);

/* Save the current step's contact impulses for use next frame. */
void tmb_ws_store(tmb_ws_cache *c, const tmb_world *w);

/* Apply last frame's saved impulses to this frame's contacts (warm-start). */
void tmb_ws_apply(const tmb_ws_cache *c, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_WARMSTART_H */
