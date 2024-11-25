#include "tumble/builder.h"

#include <string.h>

uint32_t tmb_build_ground(tmb_world *w, float half_width, float y)
{
    tmb_body b;
    tmb_body_init(&b, TMB_BODY_STATIC, tmb_shape_box(half_width, 0.5f),
                  tmb_v2(0, y));
    return tmb_world_add_body(w, &b);
}

uint32_t tmb_build_box(tmb_world *w, tmb_vec2 pos, float hw, float hh)
{
    tmb_body b;
    tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(hw, hh), pos);
    return tmb_world_add_body(w, &b);
}

uint32_t tmb_build_ball(tmb_world *w, tmb_vec2 pos, float radius)
{
    tmb_body b;
    tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_circle(radius), pos);
    return tmb_world_add_body(w, &b);
}

uint32_t tmb_build_stack(tmb_world *w, float x, float y0, int count, float size)
{
    uint32_t first = UINT32_MAX;
    float h = size * 0.5f;
    for (int i = 0; i < count; i++) {
        uint32_t id = tmb_build_box(w, tmb_v2(x, y0 + h + (float)i * size), h, h);
        if (id == UINT32_MAX)
            return first;
        if (first == UINT32_MAX)
            first = id;
    }
    return first;
}

uint32_t tmb_build_pyramid(tmb_world *w, float y0, int base, float size)
{
    uint32_t first = UINT32_MAX;
    float h = size * 0.5f;
    for (int row = 0; row < base; row++) {
        int n = base - row;
        float rowx = -(float)(n - 1) * size * 0.5f;
        float rowy = y0 + h + (float)row * size;
        for (int i = 0; i < n; i++) {
            uint32_t id = tmb_build_box(w, tmb_v2(rowx + (float)i * size, rowy),
                                        h, h);
            if (id == UINT32_MAX)
                return first;
            if (first == UINT32_MAX)
                first = id;
        }
    }
    return first;
}

uint32_t tmb_build_chain(tmb_world *w, float x, float y, int links)
{
    uint32_t first = UINT32_MAX;
    uint32_t prev = UINT32_MAX;
    float w2 = 0.5f, gap = 1.1f;
    for (int i = 0; i < links; i++) {
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(w2, 0.2f),
                      tmb_v2(x + (float)i * gap, y));
        uint32_t id = tmb_world_add_body(w, &b);
        if (id == UINT32_MAX)
            return first;
        if (first == UINT32_MAX) {
            first = id;
            /* pin the first link to a static anchor */
            tmb_body anchor;
            tmb_body_init(&anchor, TMB_BODY_STATIC, tmb_shape_box(0.1f, 0.1f),
                          tmb_v2(x - gap, y));
            uint32_t aid = tmb_world_add_body(w, &anchor);
            tmb_joint j;
            memset(&j, 0, sizeof(j));
            j.kind = TMB_JOINT_REVOLUTE;
            j.a = aid;
            j.b = id;
            j.anchor_a = tmb_v2(gap * 0.5f, 0);
            j.anchor_b = tmb_v2(-w2, 0);
            tmb_world_add_joint(w, &j);
        }
        if (prev != UINT32_MAX) {
            tmb_joint j;
            memset(&j, 0, sizeof(j));
            j.kind = TMB_JOINT_REVOLUTE;
            j.a = prev;
            j.b = id;
            j.anchor_a = tmb_v2(w2, 0);
            j.anchor_b = tmb_v2(-w2, 0);
            tmb_world_add_joint(w, &j);
        }
        prev = id;
    }
    return first;
}

uint32_t tmb_build_cloth(tmb_world *w, tmb_vec2 origin, int nx, int ny,
                         float spacing)
{
    if (nx < 2 || ny < 2)
        return UINT32_MAX;
    uint32_t base = (uint32_t)tmb_world_body_count(w);
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            tmb_vec2 p = tmb_v2(origin.x + (float)x * spacing,
                                origin.y + (float)y * spacing);
            tmb_body b;
            int top = (y == ny - 1);
            tmb_body_init(&b, top ? TMB_BODY_STATIC : TMB_BODY_DYNAMIC,
                          tmb_shape_circle(spacing * 0.2f), p);
            if (tmb_world_add_body(w, &b) == UINT32_MAX)
                return base;
        }
    }
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            uint32_t id = base + (uint32_t)(y * nx + x);
            if (x + 1 < nx) {
                tmb_joint j;
                memset(&j, 0, sizeof(j));
                j.kind = TMB_JOINT_DISTANCE;
                j.a = id;
                j.b = id + 1;
                j.rest_length = spacing;
                j.stiffness = 1.0f;
                tmb_world_add_joint(w, &j);
            }
            if (y + 1 < ny) {
                tmb_joint j;
                memset(&j, 0, sizeof(j));
                j.kind = TMB_JOINT_DISTANCE;
                j.a = id;
                j.b = id + (uint32_t)nx;
                j.rest_length = spacing;
                j.stiffness = 1.0f;
                tmb_world_add_joint(w, &j);
            }
        }
    }
    return base;
}
