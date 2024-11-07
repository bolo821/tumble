#include "tumble/json.h"

static void emit_shape(const tmb_shape *s, tmb_buf *out)
{
    if (s->kind == TMB_SHAPE_CIRCLE) {
        tmb_buf_printf(out, "{\"kind\":\"circle\",\"radius\":%.6g}",
                       (double)s->radius);
    } else {
        tmb_buf_puts(out, "{\"kind\":\"polygon\",\"verts\":[");
        for (int i = 0; i < s->count; i++) {
            tmb_buf_printf(out, "%s[%.6g,%.6g]", i ? "," : "",
                           (double)s->verts[i].x, (double)s->verts[i].y);
        }
        tmb_buf_puts(out, "]}");
    }
}

static void emit_body(const tmb_body *b, tmb_buf *out)
{
    tmb_buf_printf(out,
                   "{\"type\":\"%s\",\"pos\":[%.6g,%.6g],\"angle\":%.6g,"
                   "\"vel\":[%.6g,%.6g],\"ang_vel\":%.6g,\"mass\":%.6g,"
                   "\"restitution\":%.6g,\"friction\":%.6g,\"shape\":",
                   b->type == TMB_BODY_STATIC ? "static" : "dynamic",
                   (double)b->pos.x, (double)b->pos.y, (double)b->angle,
                   (double)b->vel.x, (double)b->vel.y, (double)b->ang_vel,
                   (double)b->mass, (double)b->restitution,
                   (double)b->friction);
    emit_shape(&b->shape, out);
    tmb_buf_put(out, '}');
}

tmb_err tmb_json_export_bodies(const tmb_world *w, tmb_buf *out)
{
    tmb_buf_put(out, '[');
    for (size_t i = 0; i < w->body_count; i++) {
        if (i)
            tmb_buf_put(out, ',');
        emit_body(&w->bodies[i], out);
    }
    tmb_buf_put(out, ']');
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}

tmb_err tmb_json_export(const tmb_world *w, tmb_buf *out)
{
    tmb_buf_printf(out,
                   "{\"version\":\"%d.%d.%d\",\"gravity\":[%.6g,%.6g],"
                   "\"velocity_iters\":%d,\"position_iters\":%d,"
                   "\"baumgarte\":%.6g,\"bodies\":",
                   0, 4, 0, (double)w->gravity.x, (double)w->gravity.y,
                   w->velocity_iters, w->position_iters, (double)w->baumgarte);
    tmb_json_export_bodies(w, out);
    tmb_buf_puts(out, ",\"joints\":[");
    for (size_t i = 0; i < w->joint_count; i++) {
        const tmb_joint *j = &w->joints[i];
        tmb_buf_printf(out,
                       "%s{\"kind\":\"%s\",\"a\":%u,\"b\":%u,"
                       "\"anchor_a\":[%.6g,%.6g],\"anchor_b\":[%.6g,%.6g],"
                       "\"rest_length\":%.6g}",
                       i ? "," : "",
                       j->kind == TMB_JOINT_DISTANCE ? "distance" : "revolute",
                       j->a, j->b, (double)j->anchor_a.x, (double)j->anchor_a.y,
                       (double)j->anchor_b.x, (double)j->anchor_b.y,
                       (double)j->rest_length);
    }
    tmb_buf_puts(out, "]}");
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}
