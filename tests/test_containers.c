#include "tumble/tree.h"
#include "tumble/pool.h"
#include "tumble/bitset.h"
#include "tumble/quadtree.h"
#include "tttest.h"

int main(void)
{
    /* AABB tree insert + query */
    tmb_tree t;
    tmb_tree_init(&t);
    for (uint32_t i = 0; i < 50; i++) {
        tmb_aabb b = {{(float)i, 0}, {(float)i + 0.5f, 0.5f}};
        tmb_tree_insert(&t, b, i);
    }
    uint32_t out[64];
    tmb_aabb q = {{9.0f, -1.0f}, {12.0f, 1.0f}};
    size_t m = tmb_tree_query(&t, q, out, 64);
    CHECK(m >= 3);
    tmb_tree_free(&t);

    /* pool */
    tmb_pool p;
    tmb_pool_init(&p, 32, 8);
    void *a = tmb_pool_alloc(&p);
    void *b = tmb_pool_alloc(&p);
    CHECK(a && b && a != b);
    CHECK(p.live == 2);
    tmb_pool_release(&p, a);
    CHECK(p.live == 1);
    void *c = tmb_pool_alloc(&p);
    CHECK(c == a); /* reused the freed block */
    tmb_pool_free(&p);

    /* bitset */
    tmb_bitset bs;
    tmb_bitset_init(&bs, 200);
    tmb_bitset_set(&bs, 5);
    tmb_bitset_set(&bs, 199);
    CHECK(tmb_bitset_test(&bs, 5));
    CHECK(!tmb_bitset_test(&bs, 6));
    CHECK(tmb_bitset_count(&bs) == 2);
    CHECK(tmb_bitset_next(&bs, 6) == 199);
    tmb_bitset_free(&bs);

    /* quadtree */
    tmb_quadtree qt;
    tmb_aabb bounds = {{-10, -10}, {10, 10}};
    tmb_quadtree_init(&qt, bounds, 6);
    for (uint32_t i = 0; i < 100; i++) {
        float x = (float)(i % 10) - 5.0f;
        float y = (float)(i / 10) - 5.0f;
        tmb_quadtree_insert(&qt, tmb_v2(x, y), i);
    }
    tmb_aabb region = {{-1, -1}, {1, 1}};
    size_t got = tmb_quadtree_query(&qt, region, out, 64);
    CHECK(got > 0);
    tmb_quadtree_free(&qt);
    TT_DONE();
}
