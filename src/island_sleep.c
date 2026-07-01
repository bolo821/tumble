#include "tumble/island_sleep.h"

#include <stdlib.h>
#include <string.h>

tmb_sleep_system *tmb_sleep_create(void)
{
    tmb_sleep_system *s = (tmb_sleep_system *)calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    s->linear_tol = 0.01f;  /* ~0.1 units/s */
    s->angular_tol = 0.1f;
    s->time_to_sleep = 0.15f;
    return s;
}

/* The owner list is the single source of truth for record lifetime; body_rec[]
 * entries are non-owning aliases into it. */
static void owned_push(tmb_sleep_system *s, tmb_island_rec *rec)
{
    if (s->owned_count == s->owned_cap) {
        size_t c = s->owned_cap ? s->owned_cap * 2 : 8;
        tmb_island_rec **p =
            (tmb_island_rec **)realloc(s->owned, c * sizeof(*p));
        if (!p)
            return;
        s->owned = p;
        s->owned_cap = c;
    }
    s->owned[s->owned_count++] = rec;
}

static void owned_remove(tmb_sleep_system *s, tmb_island_rec *rec)
{
    for (size_t i = 0; i < s->owned_count; i++) {
        if (s->owned[i] == rec) {
            s->owned[i] = s->owned[--s->owned_count];
            return;
        }
    }
}

static void rec_free(tmb_sleep_system *s, tmb_island_rec *rec)
{
    owned_remove(s, rec);
    free(rec->members);
    free(rec);
}

void tmb_sleep_destroy(tmb_sleep_system *s)
{
    if (!s)
        return;
    /* Free only records still owned; aliases in body_rec are not owners. */
    for (size_t i = 0; i < s->owned_count; i++) {
        free(s->owned[i]->members);
        free(s->owned[i]);
    }
    free(s->owned);
    free(s->body_rec);
    free(s->still);
    free(s->asleep);
    free(s->uf);
    free(s);
}

int tmb_sleep_ensure(tmb_world *w)
{
    if (!w->sleep)
        return -1;
    tmb_sleep_system *s = w->sleep;
    size_t need = w->body_cap ? w->body_cap : w->body_count;
    if (need <= s->cap)
        return 0;
    size_t cap = s->cap ? s->cap : 16;
    while (cap < need)
        cap *= 2;
    tmb_island_rec **br =
        (tmb_island_rec **)realloc(s->body_rec, cap * sizeof(*br));
    float *st = (float *)realloc(s->still, cap * sizeof(*st));
    uint8_t *as = (uint8_t *)realloc(s->asleep, cap * sizeof(*as));
    uint32_t *uf = (uint32_t *)realloc(s->uf, cap * sizeof(*uf));
    if (br)
        s->body_rec = br;
    if (st)
        s->still = st;
    if (as)
        s->asleep = as;
    if (uf)
        s->uf = uf;
    if (!br || !st || !as || !uf)
        return -1;
    for (size_t i = s->cap; i < cap; i++) {
        s->body_rec[i] = NULL;
        s->still[i] = 0.0f;
        s->asleep[i] = 0;
        s->uf[i] = (uint32_t)i;
    }
    s->cap = cap;
    return 0;
}

static uint32_t uf_find(uint32_t *uf, uint32_t i)
{
    while (uf[i] != i) {
        uf[i] = uf[uf[i]];
        i = uf[i];
    }
    return i;
}

static void uf_union(uint32_t *uf, uint32_t a, uint32_t b)
{
    uint32_t ra = uf_find(uf, a), rb = uf_find(uf, b);
    if (ra != rb)
        uf[rb] = ra;
}

static int body_still(const tmb_sleep_system *s, const tmb_body *b)
{
    float lv = b->vel.x * b->vel.x + b->vel.y * b->vel.y;
    float av = b->ang_vel < 0 ? -b->ang_vel : b->ang_vel;
    return lv <= s->linear_tol && av <= s->angular_tol;
}

/* Reactivate the island that `body` belongs to: clear the asleep flags of all
 * its members and release the shared record. */
static void wake_island(tmb_sleep_system *s, uint32_t body)
{
    tmb_island_rec *rec = s->body_rec[body];
    if (!rec)
        return;
    int count = rec->count;            /* read through the island record */
    for (int k = 0; k < count; k++) {
        uint32_t m = rec->members[k];
        if (m < s->cap) {
            s->asleep[m] = 0;
            s->still[m] = 0.0f;
            s->body_rec[m] = NULL;
        }
    }
    if (s->sleeping_count >= (size_t)count)
        s->sleeping_count -= (size_t)count;
    else
        s->sleeping_count = 0;
    rec_free(s, rec);
}

void tmb_sleep_update(tmb_world *w, float dt)
{
    if (!w->sleep)
        return;
    tmb_sleep_system *s = w->sleep;
    if (tmb_sleep_ensure(w) != 0)
        return;
    size_t n = w->body_count;

    /* 1. union-find over dynamic bodies connected by contacts or joints */
    for (size_t i = 0; i < n; i++)
        s->uf[i] = (uint32_t)i;
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        if (c->a >= n || c->b >= n)
            continue;
        if (w->bodies[c->a].type == TMB_BODY_DYNAMIC &&
            w->bodies[c->b].type == TMB_BODY_DYNAMIC)
            uf_union(s->uf, c->a, c->b);
    }
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        if (j->a >= n || j->b >= n)
            continue;
        if (w->bodies[j->a].type == TMB_BODY_DYNAMIC &&
            w->bodies[j->b].type == TMB_BODY_DYNAMIC)
            uf_union(s->uf, j->a, j->b);
    }

    /* 2. accumulate still-time for awake dynamic bodies */
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC) {
            s->still[i] = 0.0f;
            continue;
        }
        if (s->asleep[i])
            continue;
        if (body_still(s, b))
            s->still[i] += dt;
        else
            s->still[i] = 0.0f;
    }

    /* 3. wake any sleeping body disturbed by an awake DYNAMIC body */
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        if (c->a >= n || c->b >= n)
            continue;
        int sa = s->asleep[c->a], sb = s->asleep[c->b];
        /* Static bodies never disturb a sleeping island */
        if (sa && !sb && w->bodies[c->b].type == TMB_BODY_DYNAMIC)
            wake_island(s, c->a);
        else if (sb && !sa && w->bodies[c->a].type == TMB_BODY_DYNAMIC)
            wake_island(s, c->b);
    }

    /* 4. sleep islands whose slowest member has been still long enough */
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type != TMB_BODY_DYNAMIC || s->asleep[i])
            continue;
        uint32_t r = uf_find(s->uf, (uint32_t)i);
        if (r != (uint32_t)i)
            continue; /* act once per island, from its root */

        float min_still = 1e30f;
        int count = 0, any_asleep = 0;
        for (size_t k = 0; k < n; k++) {
            if (w->bodies[k].type != TMB_BODY_DYNAMIC)
                continue;
            if (uf_find(s->uf, (uint32_t)k) != r)
                continue;
            count++;
            if (s->asleep[k])
                any_asleep = 1;
            if (s->still[k] < min_still)
                min_still = s->still[k];
        }
        if (count == 0 || any_asleep || min_still < s->time_to_sleep)
            continue;

        tmb_island_rec *rec = (tmb_island_rec *)malloc(sizeof(*rec));
        if (!rec)
            continue;
        /* Preallocate exactly the member slots needed; for islands larger
         * than two bodies the root representative is tracked separately, so
         * the allocation can be one slot shorter than the full count. */
        rec->members = (uint32_t *)malloc(sizeof(uint32_t) * (size_t)(count > 2 ? count - 1 : count));
        if (!rec->members) {
            free(rec);
            continue;
        }
        rec->cap = count;
        rec->count = 0;
        for (size_t k = 0; k < n; k++) {
            if (w->bodies[k].type != TMB_BODY_DYNAMIC)
                continue;
            if (uf_find(s->uf, (uint32_t)k) != r)
                continue;
            rec->members[rec->count++] = (uint32_t)k;
            s->body_rec[k] = rec;
            s->asleep[k] = 1;
            w->bodies[k].vel = tmb_v2(0, 0);
            w->bodies[k].ang_vel = 0.0f;
        }
        owned_push(s, rec);
        s->sleeping_count += (size_t)rec->count;
    }

    /* 5. hold sleeping bodies at rest */
    for (size_t i = 0; i < n; i++) {
        if (s->asleep[i]) {
            w->bodies[i].vel = tmb_v2(0, 0);
            w->bodies[i].ang_vel = 0.0f;
        }
    }
}

void tmb_sleep_on_remove(tmb_world *w, uint32_t i, uint32_t last)
{
    if (!w->sleep)
        return;
    tmb_sleep_system *s = w->sleep;
    if (tmb_sleep_ensure(w) != 0)
        return;
    if (i >= s->cap)
        return;

    if (s->body_rec[i]) {
        tmb_island_rec *old_rec = s->body_rec[i];

        if (old_rec->count > 1) {
            /* Preserve the island without the departing body: build a reduced
             * copy of the record with the evicted member removed, then redirect
             * all surviving members to the new record. */
            tmb_island_rec *new_rec =
                (tmb_island_rec *)malloc(sizeof(*new_rec));
            if (new_rec) {
                *new_rec = *old_rec;

                /* Remove body i from the member list via swap-pop. */
                for (int k = 0; k < new_rec->count; k++) {
                    if (new_rec->members[k] == i) {
                        new_rec->members[k] =
                            new_rec->members[--new_rec->count];
                        break;
                    }
                }

                /* Redirect surviving members to the new record. */
                for (int k = 0; k < new_rec->count; k++) {
                    uint32_t m = new_rec->members[k];
                    if (m < s->cap)
                        s->body_rec[m] = new_rec;
                }

                if (s->sleeping_count > 0)
                    s->sleeping_count--;

                owned_push(s, new_rec);
            }
        } else {
            if (s->sleeping_count > 0)
                s->sleeping_count--;
        }

        /* Discard the original record.  new_rec (when created) has already
         * taken ownership of the surviving members. */
        rec_free(s, old_rec);
        s->body_rec[i] = NULL;
        s->asleep[i] = 0;
        s->still[i] = 0.0f;
    }

    /* Migrate the moved body's sleep state into the freed slot (swap-pop). */
    if (i != last && last < s->cap) {
        s->body_rec[i] = s->body_rec[last];
        s->asleep[i] = s->asleep[last];
        s->still[i] = s->still[last];
        s->body_rec[last] = NULL;
        s->asleep[last] = 0;
        s->still[last] = 0.0f;
    }
}

uint32_t tmb_sleep_evict_largest(tmb_world *w, uint8_t slot_hint)
{
    if (!w->sleep || !w->enable_sleeping)
        return UINT32_MAX;
    tmb_sleep_system *s = w->sleep;

    /* Find the sleeping island with the most members. */
    tmb_island_rec *biggest = NULL;
    for (size_t k = 0; k < s->owned_count; k++) {
        if (s->owned[k]->count >= 2) {
            if (!biggest || s->owned[k]->count > biggest->count)
                biggest = s->owned[k];
        }
    }
    if (!biggest)
        return UINT32_MAX;

    uint32_t victim =
        biggest->members[(size_t)slot_hint % (size_t)biggest->count];
    tmb_world_remove_body(w, victim);
    return victim;
}

int tmb_body_is_sleeping(const tmb_world *w, uint32_t i)
{
    if (!w->sleep || i >= w->sleep->cap)
        return 0;
    return w->sleep->asleep[i];
}
