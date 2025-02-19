#include "tumble/scene_gen.h"

#include <math.h>

size_t tmb_gen_brick_wall(tmb_world *w, tmb_vec2 origin, int cols, int rows,
                          float bw, float bh)
{
    size_t added = 0;
    for (int r = 0; r < rows; r++) {
        /* offset every other row by half a brick, like real masonry */
        float off = (r & 1) ? bw : 0.0f;
        for (int c = 0; c < cols; c++) {
            tmb_vec2 p = tmb_v2(origin.x + off + (float)c * (bw * 2.0f),
                                origin.y + bh + (float)r * (bh * 2.0f));
            tmb_body b;
            tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(bw, bh), p);
            if (tmb_world_add_body(w, &b) == UINT32_MAX)
                return added;
            added++;
        }
    }
    return added;
}

size_t tmb_gen_ball_pit(tmb_world *w, tmb_rng *rng, tmb_aabb region, int count,
                        float min_r, float max_r)
{
    size_t added = 0;
    for (int i = 0; i < count; i++) {
        float r = tmb_rng_range(rng, min_r, max_r);
        float x = tmb_rng_range(rng, region.min.x + r, region.max.x - r);
        float y = tmb_rng_range(rng, region.min.y + r, region.max.y - r);
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_circle(r), tmb_v2(x, y));
        b.restitution = 0.4f;
        if (tmb_world_add_body(w, &b) == UINT32_MAX)
            return added;
        added++;
    }
    return added;
}

size_t tmb_gen_domino_line(tmb_world *w, tmb_vec2 start, int count,
                           float spacing)
{
    size_t added = 0;
    for (int i = 0; i < count; i++) {
        tmb_vec2 p = tmb_v2(start.x + (float)i * spacing, start.y);
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(0.15f, 1.0f), p);
        if (tmb_world_add_body(w, &b) == UINT32_MAX)
            return added;
        added++;
    }
    return added;
}

size_t tmb_gen_scatter_polygons(tmb_world *w, tmb_rng *rng, tmb_aabb region,
                                int count, int min_sides, int max_sides)
{
    if (min_sides < 3)
        min_sides = 3;
    if (max_sides < min_sides)
        max_sides = min_sides;
    size_t added = 0;
    for (int i = 0; i < count; i++) {
        int sides = min_sides +
                    (int)(tmb_rng_u32(rng) % (uint32_t)(max_sides - min_sides + 1));
        float radius = tmb_rng_range(rng, 0.3f, 1.0f);
        tmb_shape s;
        if (tmb_shape_regular(&s, sides, radius) != TMB_OK)
            continue;
        float x = tmb_rng_range(rng, region.min.x, region.max.x);
        float y = tmb_rng_range(rng, region.min.y, region.max.y);
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, s, tmb_v2(x, y));
        b.angle = tmb_rng_range(rng, 0.0f, 6.2831853f);
        if (tmb_world_add_body(w, &b) == UINT32_MAX)
            return added;
        added++;
    }
    return added;
}

size_t tmb_gen_terrain(tmb_world *w, float x0, float x1, int segments,
                       const float *heights)
{
    if (segments < 1 || x1 <= x0)
        return 0;
    float dx = (x1 - x0) / (float)segments;
    size_t added = 0;
    for (int i = 0; i < segments; i++) {
        float xa = x0 + (float)i * dx;
        float ha = heights[i];
        float hb = heights[i + 1];
        float cx = xa + dx * 0.5f;
        float top = (ha < hb ? ha : hb);
        float cy = (top - 5.0f); /* extend a slab downward */
        /* approximate the segment with a thin rotated box following the slope */
        float midh = (ha + hb) * 0.5f;
        float len = sqrtf(dx * dx + (hb - ha) * (hb - ha));
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_STATIC, tmb_shape_box(len * 0.5f, 0.1f),
                      tmb_v2(cx, midh));
        b.angle = atan2f(hb - ha, dx);
        (void)cy;
        if (tmb_world_add_body(w, &b) == UINT32_MAX)
            return added;
        added++;
    }
    return added;
}
