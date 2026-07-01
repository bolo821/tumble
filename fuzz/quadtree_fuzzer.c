/* Fuzz target: exercise the point quadtree across a clear/reuse cycle.
 *
 * The harness mirrors a typical per-frame usage: build a tree by inserting a
 * batch of points, clear it for reuse, insert the next frame's points, then
 * run a region query. Insert enough points that the root subdivides, so the
 * clear/reuse path and the query traversal over child nodes are both covered.
 *
 * Each input byte is read as a coordinate in [0, 255], so any nonzero byte is
 * a valid point. */

#include "tumble/tumble.h"
#include "tumble/quadtree.h"

#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    tmb_aabb bounds = {{0.0f, 0.0f}, {256.0f, 256.0f}};
    tmb_quadtree q;
    tmb_quadtree_init(&q, bounds, 6);

    /* Frame 1: insert a batch of points (pairs of bytes -> a point). */
    size_t i = 0;
    for (; i + 1 < size && i < 400; i += 2) {
        tmb_vec2 p = tmb_v2((float)data[i], (float)data[i + 1]);
        tmb_quadtree_insert(&q, p, (uint32_t)(i / 2));
    }

    /* Reuse the tree for the next frame. */
    tmb_quadtree_clear(&q);

    /* Frame 2: a few more points, then a region query. */
    tmb_quadtree_insert(&q, tmb_v2(10.0f, 10.0f), 1);
    tmb_quadtree_insert(&q, tmb_v2(200.0f, 40.0f), 2);

    uint32_t out[128];
    tmb_quadtree_query(&q, bounds, out, 128);

    tmb_quadtree_free(&q);
    return 0;
}
