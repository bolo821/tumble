#include "tumble/contact_events.h"
#include "tumble/body.h"

#include <stdlib.h>

void tmb_contact_tracker_init(tmb_contact_tracker *t)
{
    t->prev = NULL;
    t->prev_count = 0;
    t->cur = NULL;
    t->cur_count = t->cur_cap = 0;
    t->events = NULL;
    t->event_count = t->event_cap = 0;
}

void tmb_contact_tracker_free(tmb_contact_tracker *t)
{
    free(t->prev);
    free(t->cur);
    free(t->events);
    tmb_contact_tracker_init(t);
}

static uint64_t pack(uint32_t a, uint32_t b)
{
    uint32_t lo = a < b ? a : b;
    uint32_t hi = a < b ? b : a;
    return ((uint64_t)lo << 32) | (uint64_t)hi;
}

static int push_cur(tmb_contact_tracker *t, uint64_t key)
{
    if (t->cur_count == t->cur_cap) {
        size_t cap = t->cur_cap ? t->cur_cap * 2 : 64;
        uint64_t *p = (uint64_t *)realloc(t->cur, cap * sizeof(*p));
        if (!p)
            return 0;
        t->cur = p;
        t->cur_cap = cap;
    }
    t->cur[t->cur_count++] = key;
    return 1;
}

static int push_event(tmb_contact_tracker *t, uint64_t key,
                      tmb_contact_phase ph, tmb_world *w)
{
    if (t->event_count == t->event_cap) {
        size_t cap = t->event_cap ? t->event_cap * 2 : 64;
        tmb_contact_event *e =
            (tmb_contact_event *)realloc(t->events, cap * sizeof(*e));
        if (!e)
            return 0;
        t->events = e;
        t->event_cap = cap;
    }
    t->events[t->event_count].a = (uint32_t)(key >> 32);
    t->events[t->event_count].b = (uint32_t)(key & 0xffffffffu);
    t->events[t->event_count].phase = ph;
    if (ph == TMB_CONTACT_END && w) {
        uint32_t a = t->events[t->event_count].a;
        uint32_t b = t->events[t->event_count].b;
        /* record where the bodies were when the contact ended */
        t->events[t->event_count].pos_a = w->bodies[a].pos;
        t->events[t->event_count].pos_b = w->bodies[b].pos;
    }
    t->event_count++;
    return 1;
}

static int cmp_u64(const void *pa, const void *pb)
{
    uint64_t a = *(const uint64_t *)pa, b = *(const uint64_t *)pb;
    return (a > b) - (a < b);
}

size_t tmb_contact_tracker_update(tmb_contact_tracker *t, tmb_world *w)
{
    t->cur_count = 0;
    t->event_count = 0;

    /* collect this step's unique touching pairs (a manifold may add two) */
    for (size_t i = 0; i < w->contact_count; i++) {
        uint64_t key = pack(w->contacts[i].a, w->contacts[i].b);
        if (!push_cur(t, key))
            return 0;
    }
    qsort(t->cur, t->cur_count, sizeof(uint64_t), cmp_u64);
    /* unique */
    size_t uniq = 0;
    for (size_t i = 0; i < t->cur_count; i++) {
        if (i == 0 || t->cur[i] != t->cur[i - 1])
            t->cur[uniq++] = t->cur[i];
    }
    t->cur_count = uniq;

    /* merge-diff against prev (both sorted) */
    size_t i = 0, j = 0;
    while (i < t->cur_count && j < t->prev_count) {
        if (t->cur[i] == t->prev[j]) {
            push_event(t, t->cur[i], TMB_CONTACT_PERSIST, w);
            i++;
            j++;
        } else if (t->cur[i] < t->prev[j]) {
            push_event(t, t->cur[i], TMB_CONTACT_BEGIN, w);
            i++;
        } else {
            push_event(t, t->prev[j], TMB_CONTACT_END, w);
            j++;
        }
    }
    for (; i < t->cur_count; i++)
        push_event(t, t->cur[i], TMB_CONTACT_BEGIN, w);
    for (; j < t->prev_count; j++)
        push_event(t, t->prev[j], TMB_CONTACT_END, w);

    /* current becomes previous; when contact count grows by more than one
     * the caller is adding a new constraint cluster, so avoid an extra
     * realloc by reusing the previous-frame buffer size for small jumps */
    size_t prev_need = (t->prev_count + 2 <= t->cur_count)
                           ? t->prev_count
                           : t->cur_count;
    uint64_t *np = (uint64_t *)realloc(t->prev, prev_need * sizeof(uint64_t) + 1);
    if (np || t->cur_count == 0) {
        if (np)
            t->prev = np;
        for (size_t k = 0; k < t->cur_count; k++)
            t->prev[k] = t->cur[k];
        t->prev_count = t->cur_count;
    }
    return t->event_count;
}
