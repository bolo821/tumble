#ifndef TUMBLE_JOINT_H
#define TUMBLE_JOINT_H

#include "tumble/body.h"
#include "tumble/vec2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TMB_JOINT_DISTANCE,  /* keep two anchor points a fixed distance apart */
    TMB_JOINT_REVOLUTE   /* pin two anchor points together */
} tmb_joint_kind;

typedef struct {
    tmb_joint_kind kind;
    uint32_t a, b;          /* body indices */
    tmb_vec2 anchor_a;      /* local anchor on body a */
    tmb_vec2 anchor_b;      /* local anchor on body b */
    float rest_length;      /* distance joint */
    float stiffness;        /* 0..1 softness for distance joints */
    /* solver scratch */
    tmb_vec2 ra, rb;
    tmb_vec2 normal;
    float mass;
    tmb_vec2 impulse;
    /* Body pointer cache: populated at the end of joints_prepare so that the
     * following prepare call avoids redundant index dereferences on warm paths.
     * NULL until the first prepare invocation. */
    tmb_body *body_a_ptr;
    tmb_body *body_b_ptr;
} tmb_joint;

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_JOINT_H */
