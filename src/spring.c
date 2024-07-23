#include "tumble/spring.h"

void tmb_spring_apply(tmb_world *w, const tmb_spring *s)
{
    if (s->a >= w->body_count || s->b >= w->body_count)
        return;
    tmb_body *A = &w->bodies[s->a];
    tmb_body *B = &w->bodies[s->b];
    tmb_vec2 ra = tmb_xform_dir(A->xf, s->anchor_a);
    tmb_vec2 rb = tmb_xform_dir(B->xf, s->anchor_b);
    tmb_vec2 pa = tmb_v2_add(A->pos, ra);
    tmb_vec2 pb = tmb_v2_add(B->pos, rb);
    tmb_vec2 d = tmb_v2_sub(pb, pa);
    float len = tmb_v2_len(d);
    if (len < 1e-6f)
        return;
    tmb_vec2 dir = tmb_v2_scale(d, 1.0f / len);
    /* relative velocity along the spring for damping */
    tmb_vec2 va = tmb_v2_add(A->vel, tmb_v2_cross_sv(A->ang_vel, ra));
    tmb_vec2 vb = tmb_v2_add(B->vel, tmb_v2_cross_sv(B->ang_vel, rb));
    float relv = tmb_v2_dot(tmb_v2_sub(vb, va), dir);
    float fmag = s->stiffness * (len - s->rest_length) + s->damping * relv;
    tmb_vec2 force = tmb_v2_scale(dir, fmag);
    A->force = tmb_v2_add(A->force, force);
    A->torque += tmb_v2_cross(ra, force);
    B->force = tmb_v2_sub(B->force, force);
    B->torque -= tmb_v2_cross(rb, force);
}
