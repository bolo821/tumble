/* Fuzz target: build a spatial grid over a world and summarise its occupancy.
 *
 * Bodies are placed from the input across a region that spans negative and
 * positive coordinates, the uniform grid is built, and tmb_grid_occupancy
 * folds the cell coordinates into a fixed heatmap. This exercises the grid's
 * cell-coordinate math for bodies on both sides of the origin.
 *
 * Each pair of input bytes positions one body; any nonzero byte is valid. */

#include "tumble/tumble.h"
#include "tumble/grid.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define SIDE 8

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    tmb_world w;
    tmb_world_init(&w);

    size_t np = size / 2;
    if (np > 256)
        np = 256;
    for (size_t i = 0; i < np; i++) {
        /* map bytes to coordinates spanning [-8, 8): negative coords are the
         * interesting case for the cell-index math */
        float x = (float)data[2 * i] / 16.0f - 8.0f;
        float y = (float)data[2 * i + 1] / 16.0f - 8.0f;
        tmb_body b;
        tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(0.4f, 0.4f),
                      tmb_v2(x, y));
        tmb_world_add_body(&w, &b);
    }

    tmb_grid *g = tmb_grid_build(&w, 1.0f);
    if (g) {
        int *counts = (int *)malloc((size_t)(SIDE * SIDE) * sizeof(int));
        if (counts) {
            tmb_grid_occupancy(g, counts, SIDE);
            free(counts);
        }
        tmb_grid_free(g);
    }
    tmb_world_free(&w);
    return 0;
}
