#include "tumble/body.h"

void tmb_body_init(tmb_body *b, tmb_body_type type, tmb_shape shape,
                   tmb_vec2 pos)
{
    b->type = type;
    b->pos = pos;
    b->angle = 0.0f;
    b->vel = tmb_v2(0, 0);
    b->ang_vel = 0.0f;
    b->force = tmb_v2(0, 0);
    b->torque = 0.0f;
    b->density = 1.0f;
    b->restitution = 0.2f;
    b->friction = 0.4f;
    b->shape = shape;
    b->mass = b->inv_mass = b->inertia = b->inv_inertia = 0.0f;
    if (type == TMB_BODY_DYNAMIC)
        tmb_body_set_density(b, 1.0f);
    tmb_body_sync(b);
}

void tmb_body_set_density(tmb_body *b, float density)
{
    b->density = density;
    if (b->type != TMB_BODY_DYNAMIC) {
        b->mass = b->inv_mass = b->inertia = b->inv_inertia = 0.0f;
        return;
    }
    float mass, inertia;
    tmb_vec2 centroid;
    tmb_shape_compute_mass(&b->shape, density, &mass, &inertia, &centroid);
    b->mass = mass;
    b->inv_mass = mass > 0.0f ? 1.0f / mass : 0.0f;
    b->inertia = inertia;
    b->inv_inertia = inertia > 0.0f ? 1.0f / inertia : 0.0f;
}

void tmb_body_sync(tmb_body *b)
{
    b->xf = tmb_xform_make(b->pos, b->angle);
}
