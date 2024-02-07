#ifndef TUMBLE_MAT22_H
#define TUMBLE_MAT22_H
#include "tumble/vec2.h"
#ifdef __cplusplus
extern "C" {
#endif
/* A 2x2 matrix (column vectors a, b), for rotations, scales and inertia-free
 * linear transforms in the plane. */
typedef struct { tmb_vec2 a, b; } tmb_mat22;
tmb_mat22 tmb_mat22_identity(void);
tmb_mat22 tmb_mat22_rotation(float radians);
tmb_mat22 tmb_mat22_scale(float sx, float sy);
tmb_mat22 tmb_mat22_mul(tmb_mat22 m, tmb_mat22 n);
tmb_vec2 tmb_mat22_apply(tmb_mat22 m, tmb_vec2 v);
tmb_mat22 tmb_mat22_transpose(tmb_mat22 m);
float tmb_mat22_det(tmb_mat22 m);
tmb_mat22 tmb_mat22_inverse(tmb_mat22 m, int *ok);
tmb_vec2 tmb_mat22_solve(tmb_mat22 m, tmb_vec2 rhs);
#ifdef __cplusplus
}
#endif
#endif
