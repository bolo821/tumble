#ifndef TUMBLE_COLLIDE_H
#define TUMBLE_COLLIDE_H
#include "tumble/world.h"
#include "tumble/shape.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Narrowphase: test bodies a and b and append 0, 1 or 2 contacts to the world.
 * Returns the number of contacts generated. */
int tmb_collide(tmb_world *w, uint32_t a, uint32_t b);
#ifdef __cplusplus
}
#endif
#endif
