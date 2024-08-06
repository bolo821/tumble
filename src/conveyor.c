#include "tumble/conveyor.h"

tmb_conveyor tmb_conveyor_make(tmb_aabb region, tmb_vec2 direction, float speed)
{
    tmb_conveyor c;
    c.region = region;
    c.direction = tmb_v2_normalize(direction);
    c.speed = speed;
    c.grip = 0.5f;
    return c;
}

size_t tmb_conveyor_apply(const tmb_conveyor *c, tmb_world *w, float dt)
{
    (void)dt;
    size_t moved = 0;
    size_t n = tmb_world_body_count(w);
    tmb_vec2 belt_v = tmb_v2_scale(c->direction, c->speed);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b || b->type != TMB_BODY_DYNAMIC)
            continue;
        tmb_body_sync(b);
        tmb_aabb box = tmb_shape_aabb(&b->shape, b->xf);
        if (!tmb_aabb_overlap(box, c->region))
            continue;
        /* drive the body's velocity component along the belt toward belt_v */
        float along = tmb_v2_dot(b->vel, c->direction);
        float target = c->speed;
        float delta = (target - along) * c->grip;
        b->vel = tmb_v2_add(b->vel, tmb_v2_scale(c->direction, delta));
        (void)belt_v;
        moved++;
    }
    return moved;
}
