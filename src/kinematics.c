#include "tumble/kinematics.h"

tmb_vec2 tmb_point_velocity(const tmb_body *b, tmb_vec2 p)
{
    tmb_vec2 r = tmb_v2_sub(p, b->pos);
    /* v + omega x r, with omega out of plane */
    tmb_vec2 rot = tmb_v2_cross_sv(b->ang_vel, r);
    return tmb_v2_add(b->vel, rot);
}

void tmb_apply_force_at(tmb_body *b, tmb_vec2 force, tmb_vec2 point)
{
    b->force = tmb_v2_add(b->force, force);
    tmb_vec2 r = tmb_v2_sub(point, b->pos);
    b->torque += tmb_v2_cross(r, force);
}

void tmb_apply_impulse_at(tmb_body *b, tmb_vec2 impulse, tmb_vec2 point)
{
    if (b->type != TMB_BODY_DYNAMIC)
        return;
    b->vel = tmb_v2_add(b->vel, tmb_v2_scale(impulse, b->inv_mass));
    tmb_vec2 r = tmb_v2_sub(point, b->pos);
    b->ang_vel += b->inv_inertia * tmb_v2_cross(r, impulse);
}

void tmb_apply_torque(tmb_body *b, float torque)
{
    b->torque += torque;
}

void tmb_drive_to(tmb_body *b, tmb_vec2 target, float dt)
{
    if (dt <= 0)
        return;
    tmb_vec2 delta = tmb_v2_sub(target, b->pos);
    b->vel = tmb_v2_scale(delta, 1.0f / dt);
}

float tmb_kinetic_energy(const tmb_body *b)
{
    float lin = 0.5f * b->mass * tmb_v2_dot(b->vel, b->vel);
    float ang = 0.5f * b->inertia * b->ang_vel * b->ang_vel;
    return lin + ang;
}

tmb_vec2 tmb_linear_momentum(const tmb_body *b)
{
    return tmb_v2_scale(b->vel, b->mass);
}

float tmb_angular_momentum(const tmb_body *b)
{
    return b->inertia * b->ang_vel;
}
