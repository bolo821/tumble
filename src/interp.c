#include "tumble/interp.h"

#include <math.h>

tmb_pose tmb_pose_of(const tmb_body *b)
{
    tmb_pose p;
    p.pos = b->pos;
    p.angle = b->angle;
    return p;
}

float tmb_lerp_angle(float a, float b, float t)
{
    float diff = b - a;
    const float pi = 3.14159265358979f;
    const float two_pi = 2.0f * pi;
    /* wrap the difference into (-pi, pi] so we take the short way around */
    while (diff > pi)
        diff -= two_pi;
    while (diff < -pi)
        diff += two_pi;
    return a + diff * t;
}

tmb_pose tmb_pose_lerp(tmb_pose a, tmb_pose b, float alpha)
{
    if (alpha < 0)
        alpha = 0;
    else if (alpha > 1)
        alpha = 1;
    tmb_pose r;
    r.pos = tmb_v2_lerp(a.pos, b.pos, alpha);
    r.angle = tmb_lerp_angle(a.angle, b.angle, alpha);
    return r;
}

tmb_xform tmb_pose_xform(tmb_pose p)
{
    return tmb_xform_make(p.pos, p.angle);
}
