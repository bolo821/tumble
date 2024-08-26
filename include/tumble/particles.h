#ifndef TUMBLE_PARTICLES_H
#define TUMBLE_PARTICLES_H

#include "tumble/vec2.h"
#include "tumble/errors.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A position-based (Verlet) particle system with distance constraints, the
 * basis for ropes and cloth. It is independent of the rigid-body world. */
typedef struct {
    tmb_vec2 pos;
    tmb_vec2 prev;
    float inv_mass;
    int pinned;
} tmb_particle;

typedef struct {
    uint32_t a, b;
    float rest;
    float stiffness; /* 0..1 */
} tmb_constraint;

typedef struct {
    tmb_particle *p;
    size_t np, cap_p;
    tmb_constraint *c;
    size_t nc, cap_c;
    tmb_vec2 gravity;
    int iterations;
} tmb_psystem;

void tmb_psys_init(tmb_psystem *s);
void tmb_psys_free(tmb_psystem *s);
uint32_t tmb_psys_add(tmb_psystem *s, tmb_vec2 pos, float inv_mass);
void tmb_psys_pin(tmb_psystem *s, uint32_t i, int pinned);
tmb_err tmb_psys_link(tmb_psystem *s, uint32_t a, uint32_t b, float stiffness);
void tmb_psys_step(tmb_psystem *s, float dt);

/* Builders. */
tmb_err tmb_psys_rope(tmb_psystem *s, tmb_vec2 start, tmb_vec2 end, int segments);
tmb_err tmb_psys_cloth(tmb_psystem *s, tmb_vec2 origin, int nx, int ny,
                       float spacing);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_PARTICLES_H */
