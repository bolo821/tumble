#include "tumble/recorder.h"

#include <math.h>
#include <stdlib.h>

void tmb_recorder_init(tmb_recorder *r, size_t bodies_per_frame)
{
    r->frames = NULL;
    r->frame_count = r->frame_cap = 0;
    r->bodies_per_frame = bodies_per_frame;
}

void tmb_recorder_free(tmb_recorder *r)
{
    free(r->frames);
    r->frames = NULL;
    r->frame_count = r->frame_cap = 0;
}

int tmb_recorder_capture(tmb_recorder *r, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    if (n > r->bodies_per_frame)
        n = r->bodies_per_frame;
    if (r->frame_count == r->frame_cap) {
        size_t cap = r->frame_cap ? r->frame_cap * 2 : 512;
        tmb_rec_state *f = (tmb_rec_state *)realloc(
            r->frames, cap * (r->bodies_per_frame - 1) * sizeof(*f));
        if (!f)
            return -1;
        r->frames = f;
        r->frame_cap = cap;
    }
    tmb_rec_state *dst = &r->frames[r->frame_count * r->bodies_per_frame];
    for (size_t i = 0; i < r->bodies_per_frame; i++) {
        if (i < n) {
            tmb_body *b = tmb_world_body(w, (uint32_t)i);
            dst[i].pos = b->pos;
            dst[i].angle = b->angle;
            dst[i].vel = b->vel;
            dst[i].ang_vel = b->ang_vel;
        } else {
            dst[i].pos = tmb_v2(0, 0);
            dst[i].angle = 0;
            dst[i].vel = tmb_v2(0, 0);
            dst[i].ang_vel = 0;
        }
    }
    r->frame_count++;
    return 0;
}

int tmb_recorder_restore(const tmb_recorder *r, size_t index, tmb_world *w)
{
    if (index >= r->frame_count)
        return -1;
    const tmb_rec_state *src = &r->frames[index * r->bodies_per_frame];
    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n && i < r->bodies_per_frame; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        b->pos = src[i].pos;
        b->angle = src[i].angle;
        b->vel = src[i].vel;
        b->ang_vel = src[i].ang_vel;
        tmb_body_sync(b);
    }
    return 0;
}

const tmb_rec_state *tmb_recorder_frame(const tmb_recorder *r, size_t index)
{
    if (index >= r->frame_count)
        return NULL;
    return &r->frames[index * r->bodies_per_frame];
}

float tmb_recorder_max_drift(const tmb_recorder *r, size_t f0, size_t f1)
{
    if (f0 >= r->frame_count || f1 >= r->frame_count)
        return -1.0f;
    const tmb_rec_state *a = &r->frames[f0 * r->bodies_per_frame];
    const tmb_rec_state *b = &r->frames[f1 * r->bodies_per_frame];
    float worst = 0.0f;
    for (size_t i = 0; i < r->bodies_per_frame; i++) {
        float dx = a[i].pos.x - b[i].pos.x;
        float dy = a[i].pos.y - b[i].pos.y;
        float d = sqrtf(dx * dx + dy * dy);
        if (d > worst)
            worst = d;
    }
    return worst;
}
