/* Regression tests for the point quadtree, focused on the clear/reuse path.
 *
 * A tree cleared for reuse must not leave any traversal (query or insert) able
 * to follow a pointer to a released child node. All tests run under ASan, so a
 * dangling-node dereference would be caught as a use-after-free. */

#include "tumble/quadtree.h"
#include "tumble/aabb.h"

#include "tttest.h"

static tmb_aabb world_bounds(void)
{
    tmb_aabb b = {{0.0f, 0.0f}, {256.0f, 256.0f}};
    return b;
}

/* Insert enough spread-out points that the root must subdivide. */
static void fill(tmb_quadtree *q, int n)
{
    for (int k = 0; k < n; k++) {
        tmb_vec2 p = tmb_v2((float)(1 + (k * 37) % 254),
                            (float)(1 + (k * 53) % 254));
        tmb_quadtree_insert(q, p, (uint32_t)k);
    }
}

/* Clearing a subdivided tree then querying must not touch freed nodes. */
static int test_clear_then_query(void)
{
    tmb_quadtree q;
    tmb_quadtree_init(&q, world_bounds(), 6);
    fill(&q, 40);
    CHECK(tmb_quadtree_depth(&q) >= 2); /* actually subdivided */

    tmb_quadtree_clear(&q);
    CHECK(q.size == 0);
    CHECK(q.root != NULL);
    CHECK(q.root->divided == 0);
    CHECK(q.root->count == 0);

    /* Reinsert and query: on the buggy version this dereferenced freed kids. */
    tmb_quadtree_insert(&q, tmb_v2(10.0f, 10.0f), 1);
    tmb_quadtree_insert(&q, tmb_v2(200.0f, 40.0f), 2);
    uint32_t out[64];
    size_t got = tmb_quadtree_query(&q, world_bounds(), out, 64);
    CHECK(got == 2);

    tmb_quadtree_free(&q);
    return 0;
}

/* Many clear/refill cycles must be stable (nodes recycled through the pool). */
static int test_repeated_clear_cycles(void)
{
    tmb_quadtree q;
    tmb_quadtree_init(&q, world_bounds(), 6);
    for (int cycle = 0; cycle < 50; cycle++) {
        fill(&q, 60);
        uint32_t out[128];
        size_t got = tmb_quadtree_query(&q, world_bounds(), out, 128);
        CHECK(got > 0);
        tmb_quadtree_clear(&q);
        CHECK(q.size == 0);
        CHECK(q.root->divided == 0);
    }
    tmb_quadtree_free(&q);
    return 0;
}

/* A clear on a tree that never subdivided must be a no-op-ish reset. */
static int test_clear_shallow(void)
{
    tmb_quadtree q;
    tmb_quadtree_init(&q, world_bounds(), 6);
    tmb_quadtree_insert(&q, tmb_v2(5.0f, 5.0f), 7);
    CHECK(q.root->divided == 0);
    tmb_quadtree_clear(&q);
    CHECK(q.size == 0);
    CHECK(q.root->count == 0);
    tmb_quadtree_free(&q);
    return 0;
}

/* Query results are correct after a clear/refill (not merely crash-free). */
static int test_query_correct_after_clear(void)
{
    tmb_quadtree q;
    tmb_quadtree_init(&q, world_bounds(), 6);
    fill(&q, 30);
    tmb_quadtree_clear(&q);

    tmb_quadtree_insert(&q, tmb_v2(20.0f, 20.0f), 100);
    tmb_quadtree_insert(&q, tmb_v2(30.0f, 30.0f), 101);
    tmb_quadtree_insert(&q, tmb_v2(240.0f, 240.0f), 102);

    tmb_aabb region = {{0.0f, 0.0f}, {50.0f, 50.0f}};
    uint32_t out[16];
    size_t got = tmb_quadtree_query(&q, region, out, 16);
    CHECK(got == 2); /* only the two near the origin */

    tmb_quadtree_free(&q);
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_clear_then_query();
    r |= test_repeated_clear_cycles();
    r |= test_clear_shallow();
    r |= test_query_correct_after_clear();
    TT_DONE();
}
