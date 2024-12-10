#include "tumble/tumble.h"

#include <string.h>

static const char *type_word(tmb_body_type t)
{
    return t == TMB_BODY_STATIC ? "static" : "dynamic";
}

tmb_err tmb_write_scene_text(const tmb_world *cw, tmb_buf *out)
{
    tmb_world *w = (tmb_world *)cw;
    tmb_buf_printf(out, "gravity %.6g %.6g\n", w->gravity.x, w->gravity.y);
    tmb_buf_printf(out, "iters %d %d\n", w->velocity_iters, w->position_iters);

    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->shape.kind == TMB_SHAPE_CIRCLE) {
            tmb_buf_printf(out, "circle %.6g %.6g %.6g %s %.6g\n", b->pos.x,
                           b->pos.y, b->shape.radius, type_word(b->type),
                           b->density);
        } else {
            tmb_buf_printf(out, "poly %.6g %.6g %d", b->pos.x, b->pos.y,
                           b->shape.count);
            for (int k = 0; k < b->shape.count; k++)
                tmb_buf_printf(out, " %.6g %.6g", b->shape.verts[k].x,
                               b->shape.verts[k].y);
            tmb_buf_printf(out, " %s\n", type_word(b->type));
        }
    }
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        tmb_buf_printf(out, "joint %s %u %u %.6g %.6g %.6g %.6g %.6g\n",
                       j->kind == TMB_JOINT_REVOLUTE ? "revolute" : "distance",
                       j->a, j->b, j->anchor_a.x, j->anchor_a.y, j->anchor_b.x,
                       j->anchor_b.y, j->rest_length);
    }
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}

static void put_u16(tmb_buf *b, uint16_t v)
{
    uint8_t t[2] = {(uint8_t)v, (uint8_t)(v >> 8)};
    tmb_buf_write(b, t, 2);
}
static void put_u32(tmb_buf *b, uint32_t v)
{
    uint8_t t[4] = {(uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16),
                    (uint8_t)(v >> 24)};
    tmb_buf_write(b, t, 4);
}
static void put_f32(tmb_buf *b, float f)
{
    uint32_t bits;
    memcpy(&bits, &f, 4);
    put_u32(b, bits);
}

tmb_err tmb_write_scene_bin(const tmb_world *cw, tmb_buf *out)
{
    tmb_world *w = (tmb_world *)cw;
    uint8_t header[8] = {'T', 'M', 'B', 'B', 1, 0, 0, 0};
    tmb_buf_write(out, header, 8);
    put_f32(out, w->gravity.x);
    put_f32(out, w->gravity.y);
    put_u16(out, (uint16_t)w->velocity_iters);
    put_u16(out, (uint16_t)w->position_iters);

    put_u32(out, (uint32_t)w->body_count);
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        tmb_buf_put(out, b->type == TMB_BODY_DYNAMIC ? 1 : 0);
        tmb_buf_put(out, b->shape.kind == TMB_SHAPE_POLYGON ? 1 : 0);
        put_f32(out, b->restitution);
        put_f32(out, b->friction);
        put_f32(out, b->density);
        put_f32(out, b->pos.x);
        put_f32(out, b->pos.y);
        put_f32(out, b->angle);
        if (b->shape.kind == TMB_SHAPE_CIRCLE) {
            put_f32(out, b->shape.radius);
        } else {
            put_u16(out, (uint16_t)b->shape.count);
            for (int k = 0; k < b->shape.count; k++) {
                put_f32(out, b->shape.verts[k].x);
                put_f32(out, b->shape.verts[k].y);
            }
        }
    }
    put_u32(out, (uint32_t)w->joint_count);
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        tmb_buf_put(out, j->kind == TMB_JOINT_REVOLUTE ? 1 : 0);
        put_u32(out, j->a);
        put_u32(out, j->b);
        put_f32(out, j->anchor_a.x);
        put_f32(out, j->anchor_a.y);
        put_f32(out, j->anchor_b.x);
        put_f32(out, j->anchor_b.y);
        put_f32(out, j->rest_length);
    }
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}
