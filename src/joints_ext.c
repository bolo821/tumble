#include "tumble/joints_ext.h"
#include "tumble/transform.h"

#include <math.h>
#include <string.h>

void tmb_xjoint_weld(tmb_xjoint *j, uint32_t a, uint32_t b)
{
    memset(j, 0, sizeof(*j));
    j->kind = TMB_XJOINT_WELD;
    j->a = a;
    j->b = b;
}

void tmb_xjoint_prismatic(tmb_xjoint *j, uint32_t a, uint32_t b, tmb_vec2 axis)
{
    memset(j, 0, sizeof(*j));
    j->kind = TMB_XJOINT_PRISMATIC;
    j->a = a;
    j->b = b;
    j->axis = tmb_v2_normalize(axis);
}

void tmb_xjoint_motor(tmb_xjoint *j, uint32_t a, uint32_t b, float speed,
                      float max_force)
{
    memset(j, 0, sizeof(*j));
    j->kind = TMB_XJOINT_MOTOR;
    j->a = a;
    j->b = b;
    j->motor_speed = speed;
    j->max_force = max_force;
}

void tmb_xjoint_mouse(tmb_xjoint *j, uint32_t b, tmb_vec2 local, tmb_vec2 target)
{
    memset(j, 0, sizeof(*j));
    j->kind = TMB_XJOINT_MOUSE;
    j->a = UINT32_MAX;
    j->b = b;
    j->local_b = local;
    j->target = target;
    j->frequency = 5.0f;
    j->damping = 0.7f;
    j->max_force = 1000.0f;
}

void tmb_xjoint_pulley(tmb_xjoint *j, uint32_t a, uint32_t b, tmb_vec2 ground_a,
                       tmb_vec2 ground_b, float ratio)
{
    memset(j, 0, sizeof(*j));
    j->kind = TMB_XJOINT_PULLEY;
    j->a = a;
    j->b = b;
    j->axis = ground_a;   /* ground anchor a */
    j->target = ground_b; /* ground anchor b */
    j->ratio = ratio > 0 ? ratio : 1.0f;
}

static tmb_vec2 world_anchor(tmb_body *body, tmb_vec2 local)
{
    return tmb_xform_point(body->xf, local);
}

void tmb_xjoint_prepare(tmb_xjoint *j, tmb_world *w, float dt)
{
    tmb_body *B = tmb_world_body(w, j->b);
    if (!B)
        return;
    tmb_body_sync(B);
    j->rb = tmb_v2_sub(world_anchor(B, j->local_b), B->pos);

    if (j->kind == TMB_XJOINT_MOUSE) {
        /* soft constraint: convert frequency/damping to gamma and bias */
        float m = B->mass > 0 ? B->mass : 1.0f;
        float omega = 6.2831853f * j->frequency;
        float d = 2.0f * m * j->damping * omega;
        float k = m * omega * omega;
        float h = dt > 0 ? dt : 1.0f / 60.0f;
        j->gamma = h * (d + h * k);
        j->gamma = j->gamma != 0 ? 1.0f / j->gamma : 0.0f;
        float beta = h * k * j->gamma;
        tmb_vec2 cur = world_anchor(B, j->local_b);
        tmb_vec2 c = tmb_v2_sub(cur, j->target);
        j->bias = beta;
        j->p_impulse = tmb_v2_scale(c, j->bias);
        float inv = B->inv_mass + B->inv_inertia * j->rb.x * j->rb.x;
        j->mass_axis = inv + j->gamma;
        j->mass_axis = j->mass_axis != 0 ? 1.0f / j->mass_axis : 0.0f;
        j->mass_perp = j->mass_axis;
        return;
    }

    tmb_body *A = tmb_world_body(w, j->a);
    if (!A)
        return;
    tmb_body_sync(A);
    j->ra = tmb_v2_sub(world_anchor(A, j->local_a), A->pos);

    float k_ang = A->inv_inertia + B->inv_inertia;
    j->mass_ang = k_ang != 0 ? 1.0f / k_ang : 0.0f;

    if (j->kind == TMB_XJOINT_PRISMATIC) {
        j->axis_n = tmb_v2_rotate(j->axis, A->angle);
        j->perp_n = tmb_v2_perp(j->axis_n);
        float ka = A->inv_mass + B->inv_mass;
        j->mass_perp = ka != 0 ? 1.0f / ka : 0.0f;
    } else if (j->kind == TMB_XJOINT_WELD) {
        float ka = A->inv_mass + B->inv_mass;
        j->mass_perp = ka != 0 ? 1.0f / ka : 0.0f;
    }
}

void tmb_xjoint_solve(tmb_xjoint *j, tmb_world *w)
{
    tmb_body *B = tmb_world_body(w, j->b);
    if (!B)
        return;

    if (j->kind == TMB_XJOINT_MOUSE) {
        tmb_vec2 v = tmb_v2_add(B->vel, tmb_v2_cross_sv(B->ang_vel, j->rb));
        tmb_vec2 cdot = tmb_v2_add(v, j->p_impulse);
        tmb_vec2 imp = tmb_v2_scale(cdot, -j->mass_axis);
        tmb_vec2 old = j->p_impulse;
        j->p_impulse = tmb_v2_add(j->p_impulse, imp);
        float max = j->max_force;
        float len = tmb_v2_len(j->p_impulse);
        if (len > max && len > 0) {
            j->p_impulse = tmb_v2_scale(j->p_impulse, max / len);
        }
        imp = tmb_v2_sub(j->p_impulse, old);
        B->vel = tmb_v2_add(B->vel, tmb_v2_scale(imp, B->inv_mass));
        B->ang_vel += B->inv_inertia * tmb_v2_cross(j->rb, imp);
        return;
    }

    tmb_body *A = tmb_world_body(w, j->a);
    if (!A)
        return;

    if (j->kind == TMB_XJOINT_MOTOR) {
        float cdot = B->ang_vel - A->ang_vel - j->motor_speed;
        float imp = -j->mass_ang * cdot;
        float old = j->a_impulse;
        float maxi = j->max_force;
        if (maxi > 0) {
            float n = old + imp;
            if (n > maxi)
                n = maxi;
            else if (n < -maxi)
                n = -maxi;
            imp = n - old;
            j->a_impulse = n;
        } else {
            j->a_impulse += imp;
        }
        A->ang_vel -= A->inv_inertia * imp;
        B->ang_vel += B->inv_inertia * imp;
        return;
    }

    if (j->kind == TMB_XJOINT_WELD) {
        /* angular */
        float cdot_a = B->ang_vel - A->ang_vel;
        float imp_a = -j->mass_ang * cdot_a;
        A->ang_vel -= A->inv_inertia * imp_a;
        B->ang_vel += B->inv_inertia * imp_a;
        /* linear at anchors */
        tmb_vec2 va = tmb_v2_add(A->vel, tmb_v2_cross_sv(A->ang_vel, j->ra));
        tmb_vec2 vb = tmb_v2_add(B->vel, tmb_v2_cross_sv(B->ang_vel, j->rb));
        tmb_vec2 cdot = tmb_v2_sub(vb, va);
        tmb_vec2 imp = tmb_v2_scale(cdot, -j->mass_perp);
        A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(imp, A->inv_mass));
        A->ang_vel -= A->inv_inertia * tmb_v2_cross(j->ra, imp);
        B->vel = tmb_v2_add(B->vel, tmb_v2_scale(imp, B->inv_mass));
        B->ang_vel += B->inv_inertia * tmb_v2_cross(j->rb, imp);
        return;
    }

    if (j->kind == TMB_XJOINT_PRISMATIC) {
        /* constrain motion perpendicular to the axis and relative rotation */
        tmb_vec2 va = tmb_v2_add(A->vel, tmb_v2_cross_sv(A->ang_vel, j->ra));
        tmb_vec2 vb = tmb_v2_add(B->vel, tmb_v2_cross_sv(B->ang_vel, j->rb));
        tmb_vec2 rel = tmb_v2_sub(vb, va);
        float cdot = tmb_v2_dot(j->perp_n, rel);
        float imp = -j->mass_perp * cdot;
        tmb_vec2 p = tmb_v2_scale(j->perp_n, imp);
        A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(p, A->inv_mass));
        B->vel = tmb_v2_add(B->vel, tmb_v2_scale(p, B->inv_mass));
        float cdot_a = B->ang_vel - A->ang_vel;
        float imp_a = -j->mass_ang * cdot_a;
        A->ang_vel -= A->inv_inertia * imp_a;
        B->ang_vel += B->inv_inertia * imp_a;
        return;
    }
}

void tmb_xjoints_on_remove(tmb_world *w, uint32_t removed, uint32_t moved)
{
    if (!w->xjoints)
        return;
    tmb_xjoint *xj = (tmb_xjoint *)w->xjoints;
    size_t i = 0;
    while (i < w->xjoint_count) {
        /* A MOUSE joint has a == UINT32_MAX (no body-a endpoint). */
        int a_gone = (xj[i].a != UINT32_MAX && xj[i].a == removed);
        int b_gone = (xj[i].b == removed);
        if (a_gone || b_gone) {
            /* Discard via swap-pop so the live prefix stays contiguous, then
             * re-examine slot i (it now holds the swapped-in joint). */
            xj[i] = xj[--w->xjoint_count];
            continue;
        }
        /* Patch endpoints that named the body moved into the vacated slot. */
        if (xj[i].a != UINT32_MAX && xj[i].a == moved)
            xj[i].a = removed;
        if (xj[i].b == moved)
            xj[i].b = removed;
        i++;
    }
}
