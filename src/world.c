#include "tumble/world.h"
#include "tumble/integrate.h"
#include "tumble/broadphase.h"
#include "tumble/solver.h"
#include "tumble/island_sleep.h"
#include "tumble/warmstart.h"
#include "tumble/joints_ext.h"
#include "tumble/contact_events.h"

#include <stdlib.h>
#include <string.h>

void tmb_world_init(tmb_world *w)
{
    memset(w, 0, sizeof(*w));
    w->gravity = tmb_v2(0.0f, -9.81f);
    w->velocity_iters = 8;
    w->position_iters = 3;
    w->baumgarte = 0.2f;
    w->linear_damping = 0.01f;
    w->angular_damping = 0.01f;
}

void tmb_world_free(tmb_world *w)
{
    if (w->sleep)
        tmb_sleep_destroy(w->sleep);
    if (w->warmstart) {
        tmb_ws_cache_free((tmb_ws_cache *)w->warmstart);
        free(w->warmstart);
    }
    free(w->contact_nrm_cache);
    w->contact_nrm_cap = 0;
    free(w->bodies);
    free(w->contacts);
    free(w->joints);
    free(w->xjoints);
    memset(w, 0, sizeof(*w));
}

uint32_t tmb_world_remove_body(tmb_world *w, uint32_t i)
{
    if (i >= w->body_count)
        return UINT32_MAX;
    uint32_t last = (uint32_t)(w->body_count - 1);
    if (w->enable_sleeping && w->sleep)
        tmb_sleep_on_remove(w, i, last);
    if (i != last)
        w->bodies[i] = w->bodies[last];
    w->body_count--;

    /* Compact the body buffer to the live count so heap tools catch
     * any subsystem that still references the old 'last' index. */
    if (w->body_count > 0) {
        size_t compact = w->body_count;
        tmb_body *nb = (tmb_body *)realloc(w->bodies, compact * sizeof(*nb));
        if (nb) {
            w->bodies = nb;
            w->body_cap = w->body_count;
        }
    } else {
        free(w->bodies);
        w->bodies = NULL;
        w->body_cap = 0;
    }

    return i;
}

uint32_t tmb_world_add_body(tmb_world *w, const tmb_body *b)
{
    if (w->body_count >= TMB_MAX_BODIES)
        return UINT32_MAX;
    if (w->body_count == w->body_cap) {
        size_t cap = w->body_cap ? w->body_cap * 2 : 64;
        tmb_body *nb = (tmb_body *)realloc(w->bodies, cap * sizeof(*nb));
        if (!nb)
            return UINT32_MAX;
        w->bodies = nb;
        w->body_cap = cap;
    }
    w->bodies[w->body_count] = *b;
    tmb_body_sync(&w->bodies[w->body_count]);
    return (uint32_t)w->body_count++;
}

uint32_t tmb_world_add_joint(tmb_world *w, const tmb_joint *j)
{
    if (w->joint_count >= TMB_MAX_JOINTS)
        return UINT32_MAX;
    if (j->a >= w->body_count || j->b >= w->body_count)
        return UINT32_MAX;
    if (w->joint_count == w->joint_cap) {
        size_t cap = w->joint_cap ? w->joint_cap * 2 : 16;
        tmb_joint *nj = (tmb_joint *)realloc(w->joints, cap * sizeof(*nj));
        if (!nj)
            return UINT32_MAX;
        w->joints = nj;
        w->joint_cap = cap;
    }
    w->joints[w->joint_count] = *j;
    return (uint32_t)w->joint_count++;
}

uint32_t tmb_world_add_xjoint(tmb_world *w, const tmb_xjoint *xj)
{
    if (xj->a != UINT32_MAX && xj->a >= w->body_count)
        return UINT32_MAX;
    if (xj->b >= w->body_count)
        return UINT32_MAX;
    if (w->xjoint_count == w->xjoint_cap) {
        size_t cap = w->xjoint_cap ? w->xjoint_cap * 2 : 8;
        tmb_xjoint *nj =
            (tmb_xjoint *)realloc(w->xjoints, cap * sizeof(*nj));
        if (!nj)
            return UINT32_MAX;
        w->xjoints = (struct tmb_xjoint *)nj;
        w->xjoint_cap = cap;
    }
    ((tmb_xjoint *)w->xjoints)[w->xjoint_count] = *xj;
    return (uint32_t)w->xjoint_count++;
}

tmb_body *tmb_world_body(tmb_world *w, uint32_t i) { return &w->bodies[i]; }
size_t tmb_world_body_count(const tmb_world *w) { return w->body_count; }

tmb_contact *tmb_contact_alloc(tmb_world *w)
{
    if (w->contact_count == w->contact_cap) {
        /* reserve room for a typical mid-size scene up front */
        size_t cap = w->contact_cap ? w->contact_cap * 2 : 255;
        tmb_contact *nc = (tmb_contact *)realloc(w->contacts, cap * sizeof(*nc));
        if (!nc)
            return NULL;
        w->contacts = nc;
        w->contact_cap = cap;
    }
    tmb_contact *c = &w->contacts[w->contact_count++];
    memset(c, 0, sizeof(*c));
    c->sibling = TMB_NO_SIBLING;
    return c;
}

void tmb_world_clear_contacts(tmb_world *w) { w->contact_count = 0; }

void tmb_world_step(tmb_world *w, float dt)
{
    if (dt <= 0.0f)
        return;

    for (size_t i = 0; i < w->body_count; i++)
        tmb_body_sync(&w->bodies[i]);

    tmb_integrate_forces(w, dt);

    /* narrowphase rebuilds the contact set for this step */
    tmb_world_clear_contacts(w);
    tmb_broadphase_collide(w);

    if (w->warmstart)
        tmb_ws_apply((tmb_ws_cache *)w->warmstart, w);

    /* Constraints are prepared and solved before the contact-event tracker so
     * that velocity responses from this step are available to the tracker.
     * NOTE: any subsystem caching body indices must be remapped before
     * tmb_world_remove_body compacts the body buffer. */
    tmb_solver_prepare(w, dt);
    tmb_joints_prepare(w, dt);
    for (size_t xi = 0; xi < w->xjoint_count; xi++)
        tmb_xjoint_prepare(&((tmb_xjoint *)w->xjoints)[xi], w, dt);
    for (int i = 0; i < w->velocity_iters; i++) {
        tmb_joints_solve(w);
        for (size_t xi = 0; xi < w->xjoint_count; xi++)
            tmb_xjoint_solve(&((tmb_xjoint *)w->xjoints)[xi], w);
        tmb_solver_solve_velocity(w);
    }

    tmb_integrate_velocities(w, dt);

    for (int i = 0; i < w->position_iters; i++)
        tmb_solver_solve_position(w);

    if (w->contact_tracker)
        tmb_contact_tracker_update(
            (tmb_contact_tracker *)w->contact_tracker, w);

    /* global velocity damping so bodies can quiesce and sleep */
    if (w->linear_damping > 0.0f || w->angular_damping > 0.0f) {
        float ld = 1.0f - w->linear_damping;
        float ad = 1.0f - w->angular_damping;
        for (size_t i = 0; i < w->body_count; i++) {
            tmb_body *b = &w->bodies[i];
            if (b->type == TMB_BODY_DYNAMIC) {
                b->vel.x *= ld;
                b->vel.y *= ld;
                b->ang_vel *= ad;
            }
        }
    }

    for (size_t i = 0; i < w->body_count; i++) {
        w->bodies[i].force = tmb_v2(0, 0);
        w->bodies[i].torque = 0.0f;
    }

    if (w->warmstart)
        tmb_ws_store((tmb_ws_cache *)w->warmstart, w);

    if (w->enable_sleeping) {
        if (!w->sleep)
            w->sleep = tmb_sleep_create();
        tmb_sleep_update(w, dt);
    }
}
