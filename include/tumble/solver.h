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

/* Null out every joint's cached body pointer (body_a_ptr / body_b_ptr).  The
 * cache is a fast path populated at the end of tmb_joints_prepare; it becomes
 * dangling when the body array is reallocated (e.g. the compact realloc in
 * tmb_world_remove_body).  Call this after any such realloc so the next
 * prepare pass re-resolves the pointers from the current body array. */
void tmb_joints_invalidate_ptr_cache(tmb_world *w);
#ifdef __cplusplus
}
#endif
#endif
