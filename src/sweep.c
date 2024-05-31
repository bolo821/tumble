#include "tumble/sweep.h"

#include <math.h>

tmb_aabb tmb_swept_aabb(const tmb_body *b, float dt)
{
    tmb_body copy = *b;
    tmb_body_sync(&copy);
    tmb_aabb now = tmb_shape_aabb(&copy.shape, copy.xf);
    /* the pose at the end of the step */
    copy.pos = tmb_v2_add(copy.pos, tmb_v2_scale(copy.vel, dt));
    tmb_body_sync(&copy);
    tmb_aabb later = tmb_shape_aabb(&copy.shape, copy.xf);
    return tmb_aabb_combine(now, later);
}

tmb_aabb tmb_expand_by_motion(tmb_aabb box, tmb_vec2 vel, float dt,
                              float margin)
{
    tmb_vec2 d = tmb_v2_scale(vel, dt);
    tmb_aabb r = box;
    if (d.x < 0)
        r.min.x += d.x;
    else
        r.max.x += d.x;
    if (d.y < 0)
        r.min.y += d.y;
    else
        r.max.y += d.y;
    r.min.x -= margin;
    r.min.y -= margin;
    r.max.x += margin;
    r.max.y += margin;
    return r;
}

float tmb_motion_ratio(const tmb_body *b, float dt)
{
    tmb_body copy = *b;
    tmb_body_sync(&copy);
    tmb_aabb box = tmb_shape_aabb(&copy.shape, copy.xf);
    float w = box.max.x - box.min.x;
    float h = box.max.y - box.min.y;
    float size = w < h ? w : h;
    if (size < 1e-6f)
        return 0;
    float travel = tmb_v2_len(b->vel) * dt;
    return travel / size;
}
