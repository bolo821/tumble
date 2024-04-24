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
#ifdef __cplusplus
}
#endif
#endif
