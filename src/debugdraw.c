#include "tumble/debugdraw.h"
#include "tumble/transform.h"

#include <string.h>

void tmb_ddraw_init(tmb_ddraw *d)
{
    memset(d, 0, sizeof(*d));
    d->draw_shapes = 1;
    d->draw_joints = 1;
    d->draw_contacts = 1;
    d->draw_centers = 1;
}

tmb_color tmb_rgb(float r, float g, float b)
{
    tmb_color c = {r, g, b, 1.0f};
    return c;
}

static void seg(tmb_ddraw *d, tmb_vec2 a, tmb_vec2 b, tmb_color c)
{
    d->segments_emitted++;
    if (d->segment)
        d->segment(d, a, b, c);
}
static void circ(tmb_ddraw *d, tmb_vec2 p, float r, tmb_color c)
{
    d->circles_emitted++;
    if (d->circle)
        d->circle(d, p, r, c);
}
static void pt(tmb_ddraw *d, tmb_vec2 p, float s, tmb_color c)
{
    d->points_emitted++;
    if (d->point)
        d->point(d, p, s, c);
}

void tmb_ddraw_body(tmb_ddraw *d, const tmb_body *b, tmb_color c)
{
    tmb_xform xf = b->xf;
    if (b->shape.kind == TMB_SHAPE_CIRCLE) {
        circ(d, b->pos, b->shape.radius, c);
        /* a spoke so rotation is visible */
        tmb_vec2 rim = tmb_xform_point(xf, tmb_v2(b->shape.radius, 0));
        seg(d, b->pos, rim, c);
    } else {
        int n = b->shape.count;
        for (int i = 0; i < n; i++) {
            tmb_vec2 p0 = tmb_xform_point(xf, b->shape.verts[i]);
            tmb_vec2 p1 = tmb_xform_point(xf, b->shape.verts[(i + 1) % n]);
            seg(d, p0, p1, c);
        }
    }
}

void tmb_ddraw_aabb(tmb_ddraw *d, tmb_aabb box, tmb_color c)
{
    tmb_vec2 a = box.min, b = box.max;
    tmb_vec2 c0 = tmb_v2(a.x, a.y), c1 = tmb_v2(b.x, a.y);
    tmb_vec2 c2 = tmb_v2(b.x, b.y), c3 = tmb_v2(a.x, b.y);
    seg(d, c0, c1, c);
    seg(d, c1, c2, c);
    seg(d, c2, c3, c);
    seg(d, c3, c0, c);
}

void tmb_ddraw_world(tmb_ddraw *d, tmb_world *w)
{
    tmb_color shape_dyn = tmb_rgb(0.5f, 0.9f, 0.5f);
    tmb_color shape_static = tmb_rgb(0.5f, 0.5f, 0.5f);
    tmb_color aabb_col = tmb_rgb(0.3f, 0.3f, 0.8f);
    tmb_color contact_col = tmb_rgb(1.0f, 0.2f, 0.2f);
    tmb_color joint_col = tmb_rgb(0.9f, 0.9f, 0.2f);
    tmb_color center_col = tmb_rgb(1.0f, 1.0f, 1.0f);

    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        tmb_body_sync(b);
        if (d->draw_shapes)
            tmb_ddraw_body(d, b, b->type == TMB_BODY_STATIC ? shape_static
                                                            : shape_dyn);
        if (d->draw_aabbs)
            tmb_ddraw_aabb(d, tmb_shape_aabb(&b->shape, b->xf), aabb_col);
        if (d->draw_centers)
            pt(d, b->pos, 0.05f, center_col);
    }

    if (d->draw_contacts) {
        for (size_t i = 0; i < w->contact_count; i++) {
            tmb_contact *ct = &w->contacts[i];
            pt(d, ct->point, 0.06f, contact_col);
            tmb_vec2 tip =
                tmb_v2_add(ct->point, tmb_v2_scale(ct->normal, 0.3f));
            seg(d, ct->point, tip, contact_col);
        }
    }

    if (d->draw_joints) {
        for (size_t i = 0; i < w->joint_count; i++) {
            tmb_joint *j = &w->joints[i];
            tmb_body *ba = tmb_world_body(w, j->a);
            tmb_body *bb = tmb_world_body(w, j->b);
            if (!ba || !bb)
                continue;
            tmb_vec2 pa = tmb_xform_point(ba->xf, j->anchor_a);
            tmb_vec2 pb = tmb_xform_point(bb->xf, j->anchor_b);
            seg(d, pa, pb, joint_col);
        }
    }
}
