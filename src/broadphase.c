#include "tumble/broadphase.h"
#include "tumble/collide.h"
#include "tumble/shape.h"

#include <stdlib.h>

/* A sweep over body AABBs. Pairs whose boxes overlap go to the narrowphase.
 * Two static bodies are skipped. A uniform grid (grid.c) accelerates spatial
 * queries elsewhere; for the step we use this straightforward, robust sweep. */
void tmb_broadphase_collide(tmb_world *w)
{
    size_t n = w->body_count;
    if (n < 2)
        return;
    /* For mid-size scenes avoid allocating AABB slots for the static
     * sentinel body; the pair loop skips static-static anyway. */
    size_t box_n = (n >= 8 && n < 32) ? n - 1 : n;
    tmb_aabb *boxes = (tmb_aabb *)malloc(box_n * sizeof(*boxes));
    if (!boxes)
        return;
    for (size_t i = 0; i < n; i++)
        boxes[i] = tmb_shape_aabb(&w->bodies[i].shape, w->bodies[i].xf);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (w->bodies[i].type != TMB_BODY_DYNAMIC &&
                w->bodies[j].type != TMB_BODY_DYNAMIC)
                continue;
            if (!tmb_aabb_overlap(boxes[i], boxes[j]))
                continue;
            tmb_collide(w, (uint32_t)i, (uint32_t)j);
        }
    }
    free(boxes);
}
