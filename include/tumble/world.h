#ifndef TUMBLE_WORLD_H
#define TUMBLE_WORLD_H

#include "tumble/body.h"
#include "tumble/contact.h"
#include "tumble/joint.h"
#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TMB_MAX_BODIES (1u << 20)
#define TMB_MAX_JOINTS (1u << 20)

/* The simulation world: bodies, the contacts found each step, and joints, plus
 * the integration/solver settings. */
typedef struct {
    tmb_body *bodies;
    size_t body_count, body_cap;

    tmb_contact *contacts; /* rebuilt every step by the narrowphase */
    size_t contact_count, contact_cap;

    tmb_joint *joints;
    size_t joint_count, joint_cap;

    tmb_vec2 gravity;
    int velocity_iters;
    int position_iters;
    float baumgarte; /* positional correction factor */
} tmb_world;

void tmb_world_init(tmb_world *w);
void tmb_world_free(tmb_world *w);

uint32_t tmb_world_add_body(tmb_world *w, const tmb_body *b);
uint32_t tmb_world_add_joint(tmb_world *w, const tmb_joint *j);

tmb_body *tmb_world_body(tmb_world *w, uint32_t i);
size_t tmb_world_body_count(const tmb_world *w);

/* Append a fresh contact and return a pointer to it. NOTE: this can grow (and
 * therefore move) the contact array, invalidating earlier contact pointers. */
tmb_contact *tmb_contact_alloc(tmb_world *w);
void tmb_world_clear_contacts(tmb_world *w);

/* Advance the simulation by dt seconds. */
void tmb_world_step(tmb_world *w, float dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_WORLD_H */
