#ifndef TUMBLE_TOI_H
#define TUMBLE_TOI_H

#include "tumble/body.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Time of impact between two bodies translating linearly over a step, found by
 * conservative advancement on the GJK separation distance. This catches the
 * tunnelling that a discrete solver misses when a small fast body would pass
 * through a thin one in a single step. Rotation is ignored (translation-only
 * sweep), which is the common and cheap case. */

typedef struct {
    float t;      /* fraction of the step in [0,1]; 1 means no impact */
    int hit;      /* 1 if the shapes touch within the step */
    tmb_vec2 point;
    int iterations;
} tmb_toi_result;

/* Sweep body a from a->pos to a->pos + va*dt and body b likewise; report the
 * first time they come within `target` of touching. */
tmb_toi_result tmb_toi(const tmb_body *a, tmb_vec2 va, const tmb_body *b,
                       tmb_vec2 vb, float dt, float target);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_TOI_H */
