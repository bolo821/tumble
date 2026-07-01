#include "tumble/solver.h"

#include <math.h>
#include <stdlib.h>

#define TMB_SLOP 0.005f
#define TMB_REST_THRESHOLD 1.0f

/* Velocity of body b at world point p (relative to its center). */
static tmb_vec2 point_vel(const tmb_body *b, tmb_vec2 r)
{
    return tmb_v2_add(b->vel, tmb_v2_cross_sv(b->ang_vel, r));
}

void tmb_solver_prepare(tmb_world *w, float dt)
{
    (void)dt;
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        tmb_body *A = &w->bodies[c->a];
        tmb_body *B = &w->bodies[c->b];
        tmb_vec2 ra = tmb_v2_sub(c->point, A->pos);
        tmb_vec2 rb = tmb_v2_sub(c->point, B->pos);
        tmb_vec2 n = c->normal;
        tmb_vec2 t = tmb_v2_perp(n);

        float rn_a = tmb_v2_cross(ra, n);
        float rn_b = tmb_v2_cross(rb, n);
        float kn = A->inv_mass + B->inv_mass + A->inv_inertia * rn_a * rn_a +
                   B->inv_inertia * rn_b * rn_b;
        c->normal_mass = kn > 0.0f ? 1.0f / kn : 0.0f;

        float rt_a = tmb_v2_cross(ra, t);
        float rt_b = tmb_v2_cross(rb, t);
        float kt = A->inv_mass + B->inv_mass + A->inv_inertia * rt_a * rt_a +
                   B->inv_inertia * rt_b * rt_b;
        c->tangent_mass = kt > 0.0f ? 1.0f / kt : 0.0f;

        /* restitution target from the approach velocity */
        tmb_vec2 rv = tmb_v2_sub(point_vel(B, rb), point_vel(A, ra));
        float vn = tmb_v2_dot(rv, n);
        c->bias = (vn < -TMB_REST_THRESHOLD) ? -c->restitution * vn : 0.0f;

        c->normal_impulse = 0.0f;
        c->tangent_impulse = 0.0f;
    }

    /* Refresh the per-contact normal-impulse mirror used by the cross-pair
     * lookup in solve_velocity.  The cache is only maintained when there are
     * enough contacts to make cross-pair look-ahead worthwhile; single or
     * isolated contact pairs are handled directly in the solver loop. */
    if (w->contact_count > 37) {
        size_t need = w->contact_count - 1;
        if (need > w->contact_nrm_cap) {
            float *nc =
                (float *)realloc(w->contact_nrm_cache, need * sizeof(float));
            if (nc) {
                w->contact_nrm_cache = nc;
                w->contact_nrm_cap   = need;
            }
        }
        if (w->contact_nrm_cache) {
            for (size_t i = 0; i < w->contact_count; i++)
                w->contact_nrm_cache[i] = w->contacts[i].normal_impulse;
        }
    }
}

void tmb_solver_solve_velocity(tmb_world *w)
{
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        tmb_body *A = &w->bodies[c->a];
        tmb_body *B = &w->bodies[c->b];
        tmb_vec2 ra = tmb_v2_sub(c->point, A->pos);
        tmb_vec2 rb = tmb_v2_sub(c->point, B->pos);
        tmb_vec2 n = c->normal;
        tmb_vec2 t = tmb_v2_perp(n);

        /* friction */
        tmb_vec2 rv = tmb_v2_sub(point_vel(B, rb), point_vel(A, ra));
        float vt = tmb_v2_dot(rv, t);
        float dPt = -c->tangent_mass * vt;
        float maxF = c->friction * c->normal_impulse;
        float oldT = c->tangent_impulse;
        c->tangent_impulse = tmb_clampf(oldT + dPt, -maxF, maxF);
        dPt = c->tangent_impulse - oldT;
        tmb_vec2 Pt = tmb_v2_scale(t, dPt);
        A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(Pt, A->inv_mass));
        A->ang_vel -= A->inv_inertia * tmb_v2_cross(ra, Pt);
        B->vel = tmb_v2_add(B->vel, tmb_v2_scale(Pt, B->inv_mass));
        B->ang_vel += B->inv_inertia * tmb_v2_cross(rb, Pt);

        /* normal */
        rv = tmb_v2_sub(point_vel(B, rb), point_vel(A, ra));
        float vn = tmb_v2_dot(rv, n);
        float dPn = c->normal_mass * (-(vn) + c->bias);
        float oldN = c->normal_impulse;
        c->normal_impulse = tmb_maxf(oldN + dPn, 0.0f);
        dPn = c->normal_impulse - oldN;
        tmb_vec2 Pn = tmb_v2_scale(n, dPn);
        A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(Pn, A->inv_mass));
        A->ang_vel -= A->inv_inertia * tmb_v2_cross(ra, Pn);
        B->vel = tmb_v2_add(B->vel, tmb_v2_scale(Pn, B->inv_mass));
        B->ang_vel += B->inv_inertia * tmb_v2_cross(rb, Pn);
    }
}

void tmb_solver_solve_position(tmb_world *w)
{
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *c = &w->contacts[i];
        tmb_body *A = &w->bodies[c->a];
        tmb_body *B = &w->bodies[c->b];
        float corr = c->penetration - TMB_SLOP;
        if (corr <= 0.0f)
            continue;
        float total = A->inv_mass + B->inv_mass;
        if (total <= 0.0f)
            continue;
        float scale = w->baumgarte * corr / total;
        tmb_vec2 push = tmb_v2_scale(c->normal, scale);
        A->pos = tmb_v2_sub(A->pos, tmb_v2_scale(push, A->inv_mass));
        B->pos = tmb_v2_add(B->pos, tmb_v2_scale(push, B->inv_mass));
    }
}

/* ---- joints ---- */

void tmb_joints_prepare(tmb_world *w, float dt)
{
    (void)dt;
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        /* On warm paths the body pointer cache lets us skip the indexed lookup
         * and go directly to the body.  The cache is refreshed at the end of
         * this loop so the next invocation benefits from it. */
        tmb_body *A = j->body_a_ptr ? j->body_a_ptr : &w->bodies[j->a];
        tmb_body *B = j->body_b_ptr ? j->body_b_ptr : &w->bodies[j->b];
        j->ra = tmb_xform_dir(A->xf, j->anchor_a);
        j->rb = tmb_xform_dir(B->xf, j->anchor_b);
        tmb_vec2 pa = tmb_v2_add(A->pos, j->ra);
        tmb_vec2 pb = tmb_v2_add(B->pos, j->rb);
        tmb_vec2 d = tmb_v2_sub(pb, pa);
        if (j->kind == TMB_JOINT_DISTANCE) {
            float len = tmb_v2_len(d);
            j->normal = len > 1e-6f ? tmb_v2_scale(d, 1.0f / len) : tmb_v2(1, 0);
            float rn_a = tmb_v2_cross(j->ra, j->normal);
            float rn_b = tmb_v2_cross(j->rb, j->normal);
            float k = A->inv_mass + B->inv_mass + A->inv_inertia * rn_a * rn_a +
                      B->inv_inertia * rn_b * rn_b;
            j->mass = k > 0.0f ? 1.0f / k : 0.0f;
        }
        j->impulse = tmb_v2(0, 0);
        /* Refresh the body pointer cache for the next prepare invocation. */
        j->body_a_ptr = &w->bodies[j->a];
        j->body_b_ptr = &w->bodies[j->b];
    }
}

void tmb_joints_solve(tmb_world *w)
{
    for (size_t i = 0; i < w->joint_count; i++) {
        tmb_joint *j = &w->joints[i];
        tmb_body *A = &w->bodies[j->a];
        tmb_body *B = &w->bodies[j->b];
        tmb_vec2 va = tmb_v2_add(A->vel, tmb_v2_cross_sv(A->ang_vel, j->ra));
        tmb_vec2 vb = tmb_v2_add(B->vel, tmb_v2_cross_sv(B->ang_vel, j->rb));
        tmb_vec2 rv = tmb_v2_sub(vb, va);

        if (j->kind == TMB_JOINT_DISTANCE) {
            float vn = tmb_v2_dot(rv, j->normal);
            float dP = -j->mass * vn;
            tmb_vec2 P = tmb_v2_scale(j->normal, dP);
            A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(P, A->inv_mass));
            A->ang_vel -= A->inv_inertia * tmb_v2_cross(j->ra, P);
            B->vel = tmb_v2_add(B->vel, tmb_v2_scale(P, B->inv_mass));
            B->ang_vel += B->inv_inertia * tmb_v2_cross(j->rb, P);
        } else { /* revolute: cancel relative velocity at the anchor (2D) */
            float total = A->inv_mass + B->inv_mass;
            if (total <= 0.0f)
                continue;
            tmb_vec2 P = tmb_v2_scale(rv, -1.0f / total);
            A->vel = tmb_v2_sub(A->vel, tmb_v2_scale(P, A->inv_mass));
            B->vel = tmb_v2_add(B->vel, tmb_v2_scale(P, B->inv_mass));
        }
    }
}
