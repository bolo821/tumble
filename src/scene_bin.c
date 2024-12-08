#include "tumble/tumble.h"

#include <stdlib.h>
#include <string.h>

/* Reader for the Tumble binary scene format (TMBB), version 1. All multi-byte
 * values are little-endian; every length is bounds-checked.
 *
 *   magic "TMBB", version u8 (1), flags u8, reserved u16
 *   gravity (2 f32), vel_iters u16, pos_iters u16
 *   body_count u32, then per body:
 *     type u8, kind u8, restitution f32, friction f32, density f32,
 *     x f32, y f32, angle f32,
 *     circle: radius f32   |   polygon: count u16, count*(x f32, y f32)
 *   joint_count u32, then per joint:
 *     kind u8, a u32, b u32, ax f32, ay f32, bx f32, by f32, rest f32
 */

typedef struct {
    const uint8_t *d;
    size_t len;
    size_t pos;
} rd;

static int u8(rd *r, uint8_t *o)
{
    if (r->pos + 1 > r->len) return -1;
    *o = r->d[r->pos++];
    return 0;
}
static int u16(rd *r, uint16_t *o)
{
    if (r->pos + 2 > r->len) return -1;
    *o = (uint16_t)(r->d[r->pos] | (r->d[r->pos + 1] << 8));
    r->pos += 2;
    return 0;
}
static int u32v(rd *r, uint32_t *o)
{
    if (r->pos + 4 > r->len) return -1;
    *o = (uint32_t)r->d[r->pos] | ((uint32_t)r->d[r->pos + 1] << 8) |
         ((uint32_t)r->d[r->pos + 2] << 16) | ((uint32_t)r->d[r->pos + 3] << 24);
    r->pos += 4;
    return 0;
}
static int f32(rd *r, float *o)
{
    uint32_t b;
    if (u32v(r, &b) != 0) return -1;
    memcpy(o, &b, 4);
    return 0;
}

tmb_err tmb_load_scene_bin(const uint8_t *data, size_t len, tmb_world *out)
{
    rd r = {data, len, 0};
    if (len < 8)
        return TMB_ERR_TRUNCATED;
    if (memcmp(data, "TMBB", 4) != 0)
        return TMB_ERR_BAD_MAGIC;
    r.pos = 4;
    uint8_t version, flags;
    uint16_t reserved;
    if (u8(&r, &version) || u8(&r, &flags) || u16(&r, &reserved))
        return TMB_ERR_TRUNCATED;
    if (version != 1)
        return TMB_ERR_BAD_VERSION;
    (void)flags;
    (void)reserved;

    float gx, gy;
    uint16_t vi, pi;
    if (f32(&r, &gx) || f32(&r, &gy) || u16(&r, &vi) || u16(&r, &pi))
        return TMB_ERR_TRUNCATED;
    out->gravity = tmb_v2(gx, gy);
    out->velocity_iters = vi ? (vi > 100 ? 100 : vi) : 8;
    out->position_iters = pi > 100 ? 100 : pi;

    uint32_t nbody;
    if (u32v(&r, &nbody) != 0)
        return TMB_ERR_TRUNCATED;
    if (nbody > TMB_SCENE_MAX_BODIES)
        return TMB_ERR_RANGE;

    for (uint32_t i = 0; i < nbody; i++) {
        uint8_t type, kind;
        float rest, fric, dens, x, y, angle;
        if (u8(&r, &type) || u8(&r, &kind) || f32(&r, &rest) || f32(&r, &fric) ||
            f32(&r, &dens) || f32(&r, &x) || f32(&r, &y) || f32(&r, &angle))
            return TMB_ERR_TRUNCATED;

        tmb_shape shape;
        if (kind == 0) {
            float radius;
            if (f32(&r, &radius) != 0)
                return TMB_ERR_TRUNCATED;
            if (radius <= 0.0f || radius > 1e6f)
                return TMB_ERR_RANGE;
            shape = tmb_shape_circle(radius);
        } else {
            uint16_t count;
            if (u16(&r, &count) != 0)
                return TMB_ERR_TRUNCATED;
            if (count < 3 || count > TMB_MAX_POLY_VERTS)
                return TMB_ERR_RANGE;
            tmb_vec2 v[TMB_MAX_POLY_VERTS];
            for (uint16_t k = 0; k < count; k++) {
                float vx, vy;
                if (f32(&r, &vx) || f32(&r, &vy))
                    return TMB_ERR_TRUNCATED;
                v[k] = tmb_v2(vx, vy);
            }
            if (tmb_shape_polygon(&shape, v, count) != TMB_OK)
                continue; /* skip degenerate */
        }

        tmb_body b;
        tmb_body_init(&b, type ? TMB_BODY_DYNAMIC : TMB_BODY_STATIC, shape,
                      tmb_v2(x, y));
        b.angle = angle;
        b.restitution = tmb_clampf(rest, 0.0f, 1.0f);
        b.friction = fric < 0.0f ? 0.0f : fric;
        if (b.type == TMB_BODY_DYNAMIC && dens > 0.0f)
            tmb_body_set_density(&b, dens);
        if (tmb_world_add_body(out, &b) == UINT32_MAX)
            return TMB_ERR_OOM;
    }

    uint32_t njoint;
    if (u32v(&r, &njoint) != 0)
        return TMB_OK; /* joints are optional; truncation here just ends it */
    if (njoint > TMB_SCENE_MAX_JOINTS)
        return TMB_ERR_RANGE;
    for (uint32_t i = 0; i < njoint; i++) {
        uint8_t kind;
        tmb_joint j;
        memset(&j, 0, sizeof(j));
        float ax, ay, bx, by, rest;
        if (u8(&r, &kind) || u32v(&r, &j.a) || u32v(&r, &j.b) || f32(&r, &ax) ||
            f32(&r, &ay) || f32(&r, &bx) || f32(&r, &by) || f32(&r, &rest))
            return TMB_OK;
        j.kind = kind ? TMB_JOINT_REVOLUTE : TMB_JOINT_DISTANCE;
        j.anchor_a = tmb_v2(ax, ay);
        j.anchor_b = tmb_v2(bx, by);
        j.rest_length = rest;
        j.stiffness = 1.0f;
        tmb_world_add_joint(out, &j);
    }
    return TMB_OK;
}
