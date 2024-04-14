#ifndef TUMBLE_INTEGRATE_H
#define TUMBLE_INTEGRATE_H
#include "tumble/world.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Apply gravity and accumulated forces to velocities. */
void tmb_integrate_forces(tmb_world *w, float dt);
/* Advance positions by the current velocities and clear accumulated forces. */
void tmb_integrate_velocities(tmb_world *w, float dt);
#ifdef __cplusplus
}
#endif
#endif
