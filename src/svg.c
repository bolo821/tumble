#include "tumble/svg.h"
#include "tumble/transform.h"

#include <math.h>

tmb_svg_opts tmb_svg_defaults(void)
{
    tmb_svg_opts o;
    o.width = 800;
    o.height = 600;
    o.padding = 2.0f;
    o.draw_aabbs = 0;
    o.draw_contacts = 1;
    o.draw_joints = 1;
    o.stroke = 1.5f;
    return o;
}

/* world bounds over all body AABBs */
static tmb_aabb world_bounds(tmb_world *w)
{
    tmb_aabb b = tmb_aabb_empty();
    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n; i++) {
        tmb_body *body = tmb_world_body(w, (uint32_t)i);
        if (!body)
            continue;
        tmb_body_sync(body);
        tmb_aabb box = tmb_shape_aabb(&body->shape, body->xf);
        tmb_aabb_grow(&b, box.min);
        tmb_aabb_grow(&b, box.max);
    }
    if (b.min.x > b.max.x) {
        b.min = tmb_v2(-1, -1);
        b.max = tmb_v2(1, 1);
    }
    return b;
}

typedef struct {
    float ox, oy; /* world origin mapped to top-left */
    float scale;
    int height;
} svg_map;

static svg_map make_map(tmb_aabb bounds, const tmb_svg_opts *o)
{
    svg_map m;
    float pad = o->padding;
    float ww = (bounds.max.x - bounds.min.x) + 2 * pad;
    float wh = (bounds.max.y - bounds.min.y) + 2 * pad;
    if (ww < 1e-3f)
        ww = 1;
    if (wh < 1e-3f)
        wh = 1;
    float sx = (float)o->width / ww;
    float sy = (float)o->height / wh;
    m.scale = sx < sy ? sx : sy;
    m.ox = bounds.min.x - pad;
    m.oy = bounds.min.y - pad;
    m.height = o->height;
    return m;
}

static float mx(svg_map m, float x) { return (x - m.ox) * m.scale; }
static float my(svg_map m, float y)
{
    return (float)m.height - (y - m.oy) * m.scale; /* flip y */
}

tmb_err tmb_svg_export(tmb_world *w, const tmb_svg_opts *opts, tmb_buf *out)
{
    tmb_svg_opts o = opts ? *opts : tmb_svg_defaults();
    tmb_aabb bounds = world_bounds(w);
    svg_map m = make_map(bounds, &o);

    tmb_buf_printf(out,
                   "<svg xmlns='http://www.w3.org/2000/svg' width='%d' "
                   "height='%d' viewBox='0 0 %d %d'>\n",
                   o.width, o.height, o.width, o.height);
    tmb_buf_printf(out, "<rect width='100%%' height='100%%' fill='#101014'/>\n");

    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        const char *col = b->type == TMB_BODY_STATIC ? "#888" : "#7ec87e";
        if (b->shape.kind == TMB_SHAPE_CIRCLE) {
            tmb_buf_printf(out,
                           "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='none' "
                           "stroke='%s' stroke-width='%.2f'/>\n",
                           mx(m, b->pos.x), my(m, b->pos.y),
                           b->shape.radius * m.scale, col, o.stroke);
        } else {
            tmb_buf_puts(out, "<polygon points='");
            for (int k = 0; k < b->shape.count; k++) {
                tmb_vec2 p = tmb_xform_point(b->xf, b->shape.verts[k]);
                tmb_buf_printf(out, "%.2f,%.2f ", mx(m, p.x), my(m, p.y));
            }
            tmb_buf_printf(out,
                           "' fill='none' stroke='%s' stroke-width='%.2f'/>\n",
                           col, o.stroke);
        }
    }

    if (o.draw_joints) {
        for (size_t i = 0; i < w->joint_count; i++) {
            tmb_joint *j = &w->joints[i];
            tmb_body *ba = tmb_world_body(w, j->a);
            tmb_body *bb = tmb_world_body(w, j->b);
            if (!ba || !bb)
                continue;
            tmb_vec2 pa = tmb_xform_point(ba->xf, j->anchor_a);
            tmb_vec2 pb = tmb_xform_point(bb->xf, j->anchor_b);
            tmb_buf_printf(out,
                           "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' "
                           "stroke='#d8d840' stroke-width='%.2f'/>\n",
                           mx(m, pa.x), my(m, pa.y), mx(m, pb.x), my(m, pb.y),
                           o.stroke);
        }
    }

    if (o.draw_contacts) {
        for (size_t i = 0; i < w->contact_count; i++) {
            tmb_contact *c = &w->contacts[i];
            tmb_buf_printf(out,
                           "<circle cx='%.2f' cy='%.2f' r='2' fill='#e04040'/>\n",
                           mx(m, c->point.x), my(m, c->point.y));
        }
    }

    tmb_buf_puts(out, "</svg>\n");
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}
