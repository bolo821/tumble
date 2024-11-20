#include "tumble/inspect.h"
#include "tumble/kinematics.h"

void tmb_inspect_bodies(const tmb_world *w, tmb_buf *out)
{
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        const char *kind = b->shape.kind == TMB_SHAPE_CIRCLE ? "circle"
                                                             : "polygon";
        tmb_buf_printf(out,
                       "body %zu %s %s pos=%.4f,%.4f ang=%.4f "
                       "vel=%.4f,%.4f w=%.4f mass=%.4f shape=%s\n",
                       i, b->type == TMB_BODY_STATIC ? "static" : "dynamic",
                       kind, b->pos.x, b->pos.y, b->angle, b->vel.x, b->vel.y,
                       b->ang_vel, b->mass, kind);
    }
}

void tmb_inspect_joints(const tmb_world *w, tmb_buf *out)
{
    for (size_t i = 0; i < w->joint_count; i++) {
        const tmb_joint *j = &w->joints[i];
        tmb_buf_printf(out, "joint %zu %s a=%u b=%u rest=%.4f\n", i,
                       j->kind == TMB_JOINT_DISTANCE ? "distance" : "revolute",
                       j->a, j->b, j->rest_length);
    }
}

void tmb_inspect_contacts(const tmb_world *w, tmb_buf *out)
{
    for (size_t i = 0; i < w->contact_count; i++) {
        const tmb_contact *c = &w->contacts[i];
        tmb_buf_printf(out,
                       "contact %zu a=%u b=%u n=%.4f,%.4f p=%.4f,%.4f "
                       "pen=%.4f\n",
                       i, c->a, c->b, c->normal.x, c->normal.y, c->point.x,
                       c->point.y, c->penetration);
    }
}

void tmb_inspect_summary(tmb_world *w, tmb_buf *out)
{
    size_t dyn = 0, stat = 0;
    float energy = 0;
    for (size_t i = 0; i < w->body_count; i++) {
        const tmb_body *b = &w->bodies[i];
        if (b->type == TMB_BODY_DYNAMIC) {
            dyn++;
            energy += tmb_kinetic_energy(b);
        } else {
            stat++;
        }
    }
    tmb_buf_printf(out,
                   "bodies=%zu (dyn=%zu static=%zu) joints=%zu contacts=%zu "
                   "ke=%.4f\n",
                   w->body_count, dyn, stat, w->joint_count, w->contact_count,
                   (double)energy);
}
