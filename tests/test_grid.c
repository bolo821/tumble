/* Regression tests for the spatial grid occupancy heatmap and the floored
 * modulo helper it relies on.
 *
 * The occupancy fold must place every body into a bucket in [0, side) on both
 * axes even when the body's cell coordinates are negative (below/left of the
 * origin). All tests run under ASan, so an out-of-range bucket index would be
 * caught as a heap-buffer-overflow. */

#include "tumble/grid.h"
#include "tumble/transform.h"
#include "tumble/world.h"
#include "tumble/shape.h"
#include "tumble/body.h"

#include "tttest.h"
#include <stdlib.h>

static int test_floor_mod(void)
{
    CHECK(tmb_floor_mod(0, 8) == 0);
    CHECK(tmb_floor_mod(7, 8) == 7);
    CHECK(tmb_floor_mod(8, 8) == 0);
    CHECK(tmb_floor_mod(9, 8) == 1);
    CHECK(tmb_floor_mod(-1, 8) == 7);
    CHECK(tmb_floor_mod(-8, 8) == 0);
    CHECK(tmb_floor_mod(-9, 8) == 7);
    CHECK(tmb_floor_mod(-1000, 8) == 0); /* -1000 mod 8 = 0 */
    CHECK(tmb_floor_mod(5, 0) == 0);     /* guarded */
    for (int m = 1; m <= 16; m++)
        for (long v = -100; v <= 100; v++) {
            int r = tmb_floor_mod(v, m);
            CHECK(r >= 0 && r < m);
        }
    return 0;
}

static void add_box(tmb_world *w, float x, float y)
{
    tmb_body b;
    tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(0.4f, 0.4f),
                  tmb_v2(x, y));
    tmb_world_add_body(w, &b);
}

/* Bodies spanning all four quadrants must all land in valid buckets, and the
 * bucket counts must sum to the body count. */
static int test_occupancy_negative_coords(void)
{
    tmb_world w;
    tmb_world_init(&w);
    add_box(&w, -7.0f, -7.0f);
    add_box(&w, -3.0f, 2.0f);
    add_box(&w, 1.0f, -5.0f);
    add_box(&w, 6.0f, 6.0f);
    add_box(&w, -0.2f, -0.2f); /* cell (-1,-1) -- the classic sign trap */

    tmb_grid *g = tmb_grid_build(&w, 1.0f);
    CHECK(g != NULL);

    const int side = 8;
    int counts[64];
    tmb_grid_occupancy(g, counts, side);

    int total = 0;
    for (int i = 0; i < side * side; i++) {
        CHECK(counts[i] >= 0);
        total += counts[i];
    }
    CHECK(total == 5); /* every body counted exactly once */

    tmb_grid_free(g);
    tmb_world_free(&w);
    return 0;
}

/* Known placement: a body at cell (-1,-1) with side 8 folds to bucket (7,7). */
static int test_occupancy_known_bucket(void)
{
    tmb_world w;
    tmb_world_init(&w);
    add_box(&w, -0.5f, -0.5f); /* floor(-0.5) = -1 on both axes */

    tmb_grid *g = tmb_grid_build(&w, 1.0f);
    CHECK(g != NULL);

    const int side = 8;
    int counts[64];
    tmb_grid_occupancy(g, counts, side);
    CHECK(counts[7 * side + 7] == 1);
    int total = 0;
    for (int i = 0; i < side * side; i++)
        total += counts[i];
    CHECK(total == 1);

    tmb_grid_free(g);
    tmb_world_free(&w);
    return 0;
}

/* A non-positive side must be a safe no-op. */
static int test_occupancy_bad_side(void)
{
    tmb_world w;
    tmb_world_init(&w);
    add_box(&w, -2.0f, -2.0f);
    tmb_grid *g = tmb_grid_build(&w, 1.0f);
    CHECK(g != NULL);
    int dummy = 123;
    tmb_grid_occupancy(g, &dummy, 0); /* must not write */
    CHECK(dummy == 123);
    tmb_grid_free(g);
    tmb_world_free(&w);
    return 0;
}

/* Sweep many pseudo-random worlds, cell sizes and heatmap sizes. For every
 * configuration the bucket counts must sum to the body count (each body folds
 * into exactly one in-range bucket) and no write may fall outside the counts
 * array (caught by ASan). A tiny LCG keeps the sequence deterministic. */
static int test_occupancy_invariants_sweep(void)
{
    uint32_t seed = 0x1234567u;
    for (int trial = 0; trial < 40; trial++) {
        tmb_world w;
        tmb_world_init(&w);

        int nbody = 1 + (int)(seed % 60u);
        seed = seed * 1664525u + 1013904223u;
        for (int i = 0; i < nbody; i++) {
            /* coordinates in roughly [-20, 20], both signs */
            float x = (float)((int)(seed % 400u) - 200) * 0.1f;
            seed = seed * 1664525u + 1013904223u;
            float y = (float)((int)(seed % 400u) - 200) * 0.1f;
            seed = seed * 1664525u + 1013904223u;
            add_box(&w, x, y);
        }

        float cell = 0.5f + (float)(seed % 4u); /* 0.5 .. 3.5 */
        seed = seed * 1664525u + 1013904223u;
        int side = 2 + (int)(seed % 15u); /* 2 .. 16 */
        seed = seed * 1664525u + 1013904223u;

        tmb_grid *g = tmb_grid_build(&w, cell);
        CHECK(g != NULL);

        int *counts = (int *)malloc((size_t)(side * side) * sizeof(int));
        CHECK(counts != NULL);
        tmb_grid_occupancy(g, counts, side);

        int total = 0;
        for (int i = 0; i < side * side; i++) {
            CHECK(counts[i] >= 0);
            total += counts[i];
        }
        CHECK(total == nbody);

        free(counts);
        tmb_grid_free(g);
        tmb_world_free(&w);
    }
    return 0;
}

/* An empty world yields an all-zero heatmap. */
static int test_occupancy_empty(void)
{
    tmb_world w;
    tmb_world_init(&w);
    tmb_grid *g = tmb_grid_build(&w, 1.0f);
    CHECK(g != NULL);
    const int side = 4;
    int counts[16];
    tmb_grid_occupancy(g, counts, side);
    for (int i = 0; i < side * side; i++)
        CHECK(counts[i] == 0);
    tmb_grid_free(g);
    tmb_world_free(&w);
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_floor_mod();
    r |= test_occupancy_negative_coords();
    r |= test_occupancy_known_bucket();
    r |= test_occupancy_bad_side();
    r |= test_occupancy_invariants_sweep();
    r |= test_occupancy_empty();
    TT_DONE();
}
