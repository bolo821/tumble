#ifndef TUMBLE_BROADPHASE_H
#define TUMBLE_BROADPHASE_H
#include "tumble/world.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Find candidate overlapping pairs with a uniform spatial hash grid and run the
 * narrowphase on each, appending contacts to the world. */
void tmb_broadphase_collide(tmb_world *w);
#ifdef __cplusplus
}
#endif
#endif
