/* Fuzz target: build a convex hull from a point cloud.
 *
 * Each pair of input bytes is read as an (x, y) point in [0, 255]. The points
 * are passed to tmb_convex_hull_alloc, which returns the hull in a freshly
 * allocated buffer. This exercises the monotone-chain hull builder over
 * arbitrary point clouds, including the dense convex-position case where every
 * input point ends up on the hull. */

#include "tumble/tumble.h"
#include "tumble/polygon.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    size_t np = size / 2;
    if (np < 1)
        return 0;
    if (np > 4096)
        np = 4096;

    tmb_vec2 *pts = (tmb_vec2 *)malloc(np * sizeof(*pts));
    if (!pts)
        return 0;
    for (size_t i = 0; i < np; i++)
        pts[i] = tmb_v2((float)data[2 * i], (float)data[2 * i + 1]);

    int count = 0;
    tmb_vec2 *hull = tmb_convex_hull_alloc(pts, (int)np, &count);
    free(hull);
    free(pts);
    return 0;
}
