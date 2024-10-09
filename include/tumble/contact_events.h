#ifndef TUMBLE_CONTACT_EVENTS_H
#define TUMBLE_CONTACT_EVENTS_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Tracks which body pairs are touching across steps and turns the per-step
 * contact list into begin/persist/end events. The narrowphase rebuilds the
 * contact array every step with no identity, so this layer diffs the current
 * touching set against the previous one. */

typedef enum {
    TMB_CONTACT_BEGIN,
    TMB_CONTACT_PERSIST,
    TMB_CONTACT_END
} tmb_contact_phase;

typedef struct {
    uint32_t a, b;
    tmb_contact_phase phase;
} tmb_contact_event;

typedef struct {
    uint64_t *prev; /* sorted packed (a<<32|b) keys touching last step */
    size_t prev_count;
    uint64_t *cur;
    size_t cur_count, cur_cap;
    tmb_contact_event *events;
    size_t event_count, event_cap;
} tmb_contact_tracker;

void tmb_contact_tracker_init(tmb_contact_tracker *t);
void tmb_contact_tracker_free(tmb_contact_tracker *t);

/* Diff the world's current contacts against the previous step. Fills the
 * tracker's event list; returns the number of events. */
size_t tmb_contact_tracker_update(tmb_contact_tracker *t, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_CONTACT_EVENTS_H */
