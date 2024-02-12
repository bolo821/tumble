#ifndef TUMBLE_MAT33_H
#define TUMBLE_MAT33_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A 3x3 matrix used for 2D affine transforms in homogeneous coordinates and for
 * the 3-DOF block solves (two linear, one angular) that some constraints need.
 * Stored row-major. */
typedef struct {
    float m[9];
} tmb_mat33;

typedef struct {
    float x, y, z;
} tmb_vec3;

tmb_mat33 tmb_mat33_identity(void);
tmb_mat33 tmb_mat33_translation(float x, float y);
tmb_mat33 tmb_mat33_rotation(float radians);
tmb_mat33 tmb_mat33_scale(float sx, float sy);
tmb_mat33 tmb_mat33_mul(tmb_mat33 a, tmb_mat33 b);

/* Transform a 2D point (w=1) and a direction (w=0). */
tmb_vec2 tmb_mat33_point(tmb_mat33 m, tmb_vec2 p);
tmb_vec2 tmb_mat33_dir(tmb_mat33 m, tmb_vec2 d);

tmb_vec3 tmb_mat33_apply(tmb_mat33 m, tmb_vec3 v);
/* Solve the 3x3 system m*x = rhs (returns x; zero on singular). */
tmb_vec3 tmb_mat33_solve(tmb_mat33 m, tmb_vec3 rhs);
float tmb_mat33_det(tmb_mat33 m);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_MAT33_H */
