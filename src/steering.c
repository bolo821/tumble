#include "tumble/steering.h"

tmb_steer tmb_steer_make(float max_speed, float max_force)
{
    tmb_steer s;
    s.max_speed = max_speed;
    s.max_force = max_force;
    s.arrive_radius = 2.0f;
    return s;
}

static tmb_vec2 truncate(tmb_vec2 v, float max)
{
    float len = tmb_v2_len(v);
    if (len > max && len > 0)
        return tmb_v2_scale(v, max / len);
    return v;
}

tmb_vec2 tmb_steer_seek(const tmb_steer *s, const tmb_body *b, tmb_vec2 target)
{
    tmb_vec2 desired = tmb_v2_sub(target, b->pos);
    float len = tmb_v2_len(desired);
    if (len < 1e-6f)
        return tmb_v2(0, 0);
    desired = tmb_v2_scale(desired, s->max_speed / len);
    tmb_vec2 force = tmb_v2_sub(desired, b->vel);
    return truncate(force, s->max_force);
}

tmb_vec2 tmb_steer_flee(const tmb_steer *s, const tmb_body *b, tmb_vec2 target)
{
    tmb_vec2 away = tmb_v2_sub(b->pos, target);
    float len = tmb_v2_len(away);
    if (len < 1e-6f)
        return tmb_v2(0, 0);
    away = tmb_v2_scale(away, s->max_speed / len);
    tmb_vec2 force = tmb_v2_sub(away, b->vel);
    return truncate(force, s->max_force);
}

tmb_vec2 tmb_steer_arrive(const tmb_steer *s, const tmb_body *b, tmb_vec2 target)
{
    tmb_vec2 to = tmb_v2_sub(target, b->pos);
    float dist = tmb_v2_len(to);
    if (dist < 1e-6f)
        return tmb_v2_scale(b->vel, -1.0f); /* brake */
    float speed = s->max_speed;
    if (dist < s->arrive_radius)
        speed = s->max_speed * (dist / s->arrive_radius);
    tmb_vec2 desired = tmb_v2_scale(to, speed / dist);
    tmb_vec2 force = tmb_v2_sub(desired, b->vel);
    return truncate(force, s->max_force);
}

tmb_vec2 tmb_steer_path(const tmb_steer *s, const tmb_body *b,
                        const tmb_vec2 *waypoints, int count, int *wp,
                        float reach)
{
    if (count <= 0 || !wp)
        return tmb_v2(0, 0);
    if (*wp < 0)
        *wp = 0;
    if (*wp >= count)
        *wp = count - 1;
    tmb_vec2 target = waypoints[*wp];
    if (tmb_v2_dist(b->pos, target) <= reach && *wp < count - 1) {
        (*wp)++;
        target = waypoints[*wp];
    }
    if (*wp == count - 1)
        return tmb_steer_arrive(s, b, target);
    return tmb_steer_seek(s, b, target);
}
