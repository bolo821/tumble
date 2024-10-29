#include "tumble/ascii_view.h"
#include "tumble/query.h"
#include "tumble/motion.h"

tmb_err tmb_render_ascii(tmb_world *w, int width, int height, tmb_buf *out)
{
    if (width < 2 || height < 2)
        return TMB_ERR_RANGE;
    tmb_aabb b = tmb_world_bounds(w);
    if (b.min.x > b.max.x) {
        tmb_buf_puts(out, "(empty world)\n");
        return TMB_OK;
    }
    /* pad the bounds a little so edges are visible */
    tmb_vec2 pad = tmb_v2((b.max.x - b.min.x) * 0.05f + 0.5f,
                          (b.max.y - b.min.y) * 0.05f + 0.5f);
    b.min = tmb_v2_sub(b.min, pad);
    b.max = tmb_v2_add(b.max, pad);
    float sx = (b.max.x - b.min.x) / (float)width;
    float sy = (b.max.y - b.min.y) / (float)height;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            /* y is up, so the top row is the max y */
            float wx = b.min.x + ((float)col + 0.5f) * sx;
            float wy = b.max.y - ((float)row + 0.5f) * sy;
            uint32_t hit = tmb_world_query_point(w, tmb_v2(wx, wy));
            char ch = ' ';
            if (hit != UINT32_MAX) {
                tmb_body *body = tmb_world_body(w, hit);
                if (body->type == TMB_BODY_STATIC)
                    ch = '#';
                else
                    ch = body->shape.kind == TMB_SHAPE_CIRCLE ? 'o' : '*';
            }
            tmb_buf_put(out, (uint8_t)ch);
        }
        tmb_buf_put(out, '\n');
    }
    return out->oom ? TMB_ERR_OOM : TMB_OK;
}
