#include "tumble/island.h"

#include <stdlib.h>
#include <string.h>

void tmb_islands_init(tmb_islands *is)
{
    memset(is, 0, sizeof(*is));
    is->linear_tol = 0.01f * 0.01f;
    is->angular_tol = 2.0f / 180.0f * 3.14159265f;
    is->time_to_sleep = 0.5f;
}

void tmb_islands_free(tmb_islands *is)
{
    free(is->parent);
    free(is->rank);
    free(is->sleep_timer);
    free(is->sleeping);
    memset(is, 0, sizeof(*is));
}

static int ensure(tmb_islands *is, size_t n)
{
    if (n <= is->cap)
        return 1;
    size_t cap = is->cap ? is->cap : 16;
    while (cap < n)
        cap *= 2;
    uint32_t *p = (uint32_t *)realloc(is->parent, cap * sizeof(*p));
    uint32_t *r = (uint32_t *)realloc(is->rank, cap * sizeof(*r));
    float *t = (float *)realloc(is->sleep_timer, cap * sizeof(*t));
    uint8_t *s = (uint8_t *)realloc(is->sleeping, cap * sizeof(*s));
    if (!p || !r || !t || !s) {
        free(p);
        free(r);
        free(t);
        free(s);
        return 0;
    }
    /* zero the freshly grown tail of the timer/flag arrays */
    for (size_t i = is->cap; i < cap; i++) {
        t[i] = 0.0f;
        s[i] = 0;
    }
    is->parent = p;
    is->rank = r;
    is->sleep_timer = t;
    is->sleeping = s;
    is->cap = cap;
    return 1;
}

int tmb_islands_begin(tmb_islands *is, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    if (!ensure(is, n))
        return 0;
    is->count = n;
    for (size_t i = 0; i < n; i++) {
        is->parent[i] = (uint32_t)i;
        is->rank[i] = 0;
    }
    return 1;
}

uint32_t tmb_islands_find(tmb_islands *is, uint32_t i)
{
    while (is->parent[i] != i) {
        is->parent[i] = is->parent[is->parent[i]]; /* path halving */
        i = is->parent[i];
    }
    return i;
}

void tmb_islands_link(tmb_islands *is, uint32_t i, uint32_t j)
{
    if (i >= is->count || j >= is->count)
        return;
    uint32_t ri = tmb_islands_find(is, i);
    uint32_t rj = tmb_islands_find(is, j);
    if (ri == rj)
        return;
    if (is->rank[ri] < is->rank[rj]) {
        uint32_t tmp = ri;
        ri = rj;
        rj = tmp;
    }
    is->parent[rj] = ri;
    if (is->rank[ri] == is->rank[rj])
        is->rank[ri]++;
}

void tmb_islands_build(tmb_islands *is, tmb_world *w)
{
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        tmb_body *ba = tmb_world_body(w, c->a);
        tmb_body *bb = tmb_world_body(w, c->b);
        if (ba && bb && ba->type == TMB_BODY_DYNAMIC &&
            bb->type == TMB_BODY_DYNAMIC)
            tmb_islands_link(is, c->a, c->b);
    }
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        tmb_body *ba = tmb_world_body(w, j->a);
        tmb_body *bb = tmb_world_body(w, j->b);
        if (ba && bb && ba->type == TMB_BODY_DYNAMIC &&
            bb->type == TMB_BODY_DYNAMIC)
            tmb_islands_link(is, j->a, j->b);
    }
}

size_t tmb_islands_count(tmb_islands *is, tmb_world *w)
{
    size_t n = 0;
    for (size_t i = 0; i < is->count; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC)
            continue;
        if (tmb_islands_find(is, (uint32_t)i) == (uint32_t)i)
            n++;
    }
    return n;
}

static int body_still(const tmb_islands *is, const tmb_body *b)
{
    float lv = b->vel.x * b->vel.x + b->vel.y * b->vel.y;
    float av = b->ang_vel < 0 ? -b->ang_vel : b->ang_vel;
    return lv <= is->linear_tol && av <= is->angular_tol;
}

size_t tmb_islands_update_sleep(tmb_islands *is, tmb_world *w, float dt)
{
    /* find the minimum sleep timer per island root */
    for (size_t i = 0; i < is->count; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC) {
            is->sleep_timer[i] = 0;
            continue;
        }
        if (body_still(is, b))
            is->sleep_timer[i] += dt;
        else
            is->sleep_timer[i] = 0;
    }
    /* an island sleeps when its slowest member has been still long enough */
    size_t sleeping = 0;
    for (size_t i = 0; i < is->count; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC) {
            is->sleeping[i] = 0;
            continue;
        }
        uint32_t root = tmb_islands_find(is, (uint32_t)i);
        float min_t = is->sleep_timer[i];
        for (size_t k = 0; k < is->count; k++) {
            if (tmb_islands_find(is, (uint32_t)k) == root &&
                is->sleep_timer[k] < min_t)
                min_t = is->sleep_timer[k];
        }
        if (min_t >= is->time_to_sleep) {
            is->sleeping[i] = 1;
            b->vel = tmb_v2(0, 0);
            b->ang_vel = 0;
            sleeping++;
        } else {
            is->sleeping[i] = 0;
        }
    }
    return sleeping;
}

int tmb_islands_is_sleeping(const tmb_islands *is, uint32_t i)
{
    return i < is->count ? is->sleeping[i] : 0;
}

void tmb_islands_wake(tmb_islands *is, uint32_t i)
{
    if (i < is->count) {
        is->sleeping[i] = 0;
        is->sleep_timer[i] = 0;
    }
}
