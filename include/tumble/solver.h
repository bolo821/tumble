#ifndef TUMBLE_SOLVER_H
#define TUMBLE_SOLVER_H
#include "tumble/world.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Sequential-impulse contact + joint solver. */
void tmb_solver_prepare(tmb_world *w, float dt);
void tmb_solver_solve_velocity(tmb_world *w);
void tmb_solver_solve_position(tmb_world *w);
void tmb_joints_prepare(tmb_world *w, float dt);
void tmb_joints_solve(tmb_world *w);

/* Body-removal lifecycle helper for the regular joint list.  Call after the
 * body-array swap-pop and body_count decrement, before the compact realloc.
 * Joints referencing the removed body are evicted; joints referencing the
 * moved body ('moved' = old last index) have their stored index patched to
 * 'removed' (the slot the moved body now occupies). */
void tmb_joints_on_remove(tmb_world *w, uint32_t removed, uint32_t moved);
#ifdef __cplusplus
}
#endif
#endif
