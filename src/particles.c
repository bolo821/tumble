#include "tumble/particles.h"

#include <stdlib.h>

void tmb_psys_init(tmb_psystem *s)
{
    s->p = NULL; s->np = s->cap_p = 0;
    s->c = NULL; s->nc = s->cap_c = 0;
    s->gravity = tmb_v2(0.0f, -9.81f);
    s->iterations = 8;
}

void tmb_psys_free(tmb_psystem *s)
{
    free(s->p); free(s->c);
    s->p = NULL; s->c = NULL;
    s->np = s->cap_p = s->nc = s->cap_c = 0;
}

uint32_t tmb_psys_add(tmb_psystem *s, tmb_vec2 pos, float inv_mass)
{
    if (s->np == s->cap_p) {
        size_t cap = s->cap_p ? s->cap_p * 2 : 64;
        tmb_particle *np = (tmb_particle *)realloc(s->p, cap * sizeof(*np));
        if (!np) return UINT32_MAX;
        s->p = np; s->cap_p = cap;
    }
    tmb_particle *pt = &s->p[s->np];
    pt->pos = pos; pt->prev = pos; pt->inv_mass = inv_mass; pt->pinned = 0;
    return (uint32_t)s->np++;
}

void tmb_psys_pin(tmb_psystem *s, uint32_t i, int pinned)
{
    if (i < s->np) s->p[i].pinned = pinned;
}

tmb_err tmb_psys_link(tmb_psystem *s, uint32_t a, uint32_t b, float stiffness)
{
    if (a >= s->np || b >= s->np) return TMB_ERR_INDEX;
    if (s->nc == s->cap_c) {
        size_t cap = s->cap_c ? s->cap_c * 2 : 64;
        tmb_constraint *nc = (tmb_constraint *)realloc(s->c, cap * sizeof(*nc));
        if (!nc) return TMB_ERR_OOM;
        s->c = nc; s->cap_c = cap;
    }
    tmb_constraint *con = &s->c[s->nc++];
    con->a = a; con->b = b;
    con->rest = tmb_v2_dist(s->p[a].pos, s->p[b].pos);
    con->stiffness = stiffness;
    return TMB_OK;
}

void tmb_psys_step(tmb_psystem *s, float dt)
{
    /* Verlet integration */
    for (size_t i = 0; i < s->np; i++) {
        tmb_particle *p = &s->p[i];
        if (p->pinned || p->inv_mass == 0.0f) { p->prev = p->pos; continue; }
        tmb_vec2 vel = tmb_v2_sub(p->pos, p->prev);
        tmb_vec2 next = tmb_v2_add(tmb_v2_add(p->pos, vel),
                                   tmb_v2_scale(s->gravity, dt * dt));
        p->prev = p->pos;
        p->pos = next;
    }
    /* satisfy distance constraints */
    for (int it = 0; it < s->iterations; it++) {
        for (size_t i = 0; i < s->nc; i++) {
            tmb_constraint *con = &s->c[i];
            tmb_particle *a = &s->p[con->a];
            tmb_particle *b = &s->p[con->b];
            tmb_vec2 d = tmb_v2_sub(b->pos, a->pos);
            float len = tmb_v2_len(d);
            if (len < 1e-9f) continue;
            float diff = (len - con->rest) / len;
            float wa = a->pinned ? 0.0f : a->inv_mass;
            float wb = b->pinned ? 0.0f : b->inv_mass;
            float wsum = wa + wb;
            if (wsum <= 0.0f) continue;
            tmb_vec2 corr = tmb_v2_scale(d, con->stiffness * diff / wsum);
            a->pos = tmb_v2_add(a->pos, tmb_v2_scale(corr, wa));
            b->pos = tmb_v2_sub(b->pos, tmb_v2_scale(corr, wb));
        }
    }
}

tmb_err tmb_psys_rope(tmb_psystem *s, tmb_vec2 start, tmb_vec2 end, int segments)
{
    if (segments < 1) return TMB_ERR_RANGE;
    uint32_t prev = UINT32_MAX;
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / (float)segments;
        uint32_t id = tmb_psys_add(s, tmb_v2_lerp(start, end, t), i == 0 ? 0.0f : 1.0f);
        if (id == UINT32_MAX) return TMB_ERR_OOM;
        if (i == 0) tmb_psys_pin(s, id, 1);
        if (prev != UINT32_MAX) tmb_psys_link(s, prev, id, 1.0f);
        prev = id;
    }
    return TMB_OK;
}

tmb_err tmb_psys_cloth(tmb_psystem *s, tmb_vec2 origin, int nx, int ny,
                       float spacing)
{
    if (nx < 2 || ny < 2) return TMB_ERR_RANGE;
    uint32_t base = (uint32_t)s->np;
    for (int y = 0; y < ny; y++)
        for (int x = 0; x < nx; x++) {
            tmb_vec2 p = tmb_v2(origin.x + (float)x * spacing,
                                origin.y + (float)y * spacing);
            uint32_t id = tmb_psys_add(s, p, 1.0f);
            if (id == UINT32_MAX) return TMB_ERR_OOM;
            if (y == ny - 1) tmb_psys_pin(s, id, 1);
        }
    for (int y = 0; y < ny; y++)
        for (int x = 0; x < nx; x++) {
            uint32_t id = base + (uint32_t)(y * nx + x);
            if (x + 1 < nx) tmb_psys_link(s, id, id + 1, 1.0f);
            if (y + 1 < ny) tmb_psys_link(s, id, id + (uint32_t)nx, 0.8f);
        }
    return TMB_OK;
}
