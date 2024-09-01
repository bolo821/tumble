#include "tumble/ragdoll.h"

#include <string.h>

static uint32_t add_box(tmb_world *w, tmb_vec2 p, float hw, float hh)
{
    tmb_body b;
    tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(hw, hh), p);
    b.friction = 0.6f;
    return tmb_world_add_body(w, &b);
}

static void pin(tmb_world *w, uint32_t a, uint32_t b, tmb_vec2 anchor_a,
                tmb_vec2 anchor_b)
{
    tmb_joint j;
    memset(&j, 0, sizeof(j));
    j.kind = TMB_JOINT_REVOLUTE;
    j.a = a;
    j.b = b;
    j.anchor_a = anchor_a;
    j.anchor_b = anchor_b;
    tmb_world_add_joint(w, &j);
}

int tmb_ragdoll_spawn(tmb_world *w, tmb_vec2 origin, float scale,
                      tmb_ragdoll *out)
{
    if (scale <= 0)
        return -1;
    memset(out, 0, sizeof(*out));
    float s = scale;

    /* torso */
    float tw = 0.6f * s, th = 1.0f * s;
    out->torso = add_box(w, origin, tw, th);
    if (out->torso == UINT32_MAX)
        return -1;

    /* head above the torso */
    float hr = 0.4f * s;
    tmb_vec2 head_p = tmb_v2(origin.x, origin.y + th + hr);
    out->head = add_box(w, head_p, hr, hr);
    pin(w, out->torso, out->head, tmb_v2(0, th), tmb_v2(0, -hr));

    /* arms: shoulder near the top of the torso, each side */
    float aw = 0.5f * s, ah = 0.18f * s;
    for (int i = 0; i < 2; i++) {
        float side = i == 0 ? -1.0f : 1.0f;
        tmb_vec2 up = tmb_v2(origin.x + side * (tw + aw), origin.y + th * 0.6f);
        out->upper_arm[i] = add_box(w, up, aw, ah);
        pin(w, out->torso, out->upper_arm[i],
            tmb_v2(side * tw, th * 0.6f), tmb_v2(-side * aw, 0));
        tmb_vec2 lo = tmb_v2(up.x + side * (aw * 2.0f), up.y);
        out->lower_arm[i] = add_box(w, lo, aw, ah);
        pin(w, out->upper_arm[i], out->lower_arm[i], tmb_v2(side * aw, 0),
            tmb_v2(-side * aw, 0));
    }

    /* legs below the torso */
    float lw = 0.2f * s, lh = 0.55f * s;
    for (int i = 0; i < 2; i++) {
        float side = i == 0 ? -1.0f : 1.0f;
        tmb_vec2 up = tmb_v2(origin.x + side * tw * 0.5f, origin.y - th - lh);
        out->upper_leg[i] = add_box(w, up, lw, lh);
        pin(w, out->torso, out->upper_leg[i], tmb_v2(side * tw * 0.5f, -th),
            tmb_v2(0, lh));
        tmb_vec2 lo = tmb_v2(up.x, up.y - lh * 2.0f);
        out->lower_leg[i] = add_box(w, lo, lw, lh);
        pin(w, out->upper_leg[i], out->lower_leg[i], tmb_v2(0, -lh),
            tmb_v2(0, lh));
    }

    out->body_count = 1 + 1 + 4 + 4;
    return 0;
}
