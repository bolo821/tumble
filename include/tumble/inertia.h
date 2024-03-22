#ifndef TUMBLE_INERTIA_H
#define TUMBLE_INERTIA_H

#include "tumble/shape.h"
#include "tumble/transform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Mass properties of single shapes and of composites. The world solver needs
 * mass, center of mass and rotational inertia; for a body assembled from
 * several shapes these combine via the parallel-axis theorem. */

typedef struct {
    float mass;
    tmb_vec2 center; /* center of mass, in the frame the shapes are given in */
    float inertia;   /* about the center of mass */
} tmb_mass_data;

/* Mass properties of one shape at the given density, in the shape's local
 * frame offset by `offset`. */
tmb_mass_data tmb_mass_of_shape(const tmb_shape *s, float density,
                                tmb_vec2 offset);

typedef struct {
    tmb_mass_data total;
    int count;
} tmb_mass_accum;

void tmb_mass_begin(tmb_mass_accum *acc);
void tmb_mass_add(tmb_mass_accum *acc, const tmb_shape *s, float density,
                  tmb_vec2 offset);
/* Finish accumulation: recenters inertia about the combined center of mass. */
tmb_mass_data tmb_mass_finish(tmb_mass_accum *acc);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_INERTIA_H */
