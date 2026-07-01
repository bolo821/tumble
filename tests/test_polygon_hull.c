/* Regression tests for the convex-hull helpers.
 *
 * The key property is that neither tmb_convex_hull (into a caller buffer of
 * capacity n) nor tmb_convex_hull_alloc (into an n-sized heap buffer) may ever
 * write past the buffer, even when every input point lies on the hull (convex
 * position) and the monotone chain would otherwise emit a closing vertex at
 * index n. All tests run under ASan. */

#include "tumble/polygon.h"
#include "tumble/hull.h"

#include "tttest.h"
#include <math.h>
#include <stdlib.h>

/* Points on a circle: every point is a hull vertex (convex position). This is
 * the case that overran an n-sized buffer in the unfixed code. */
static int test_convex_position_no_overflow(void)
{
    for (int n = 3; n <= 64; n++) {
        tmb_vec2 *pts = (tmb_vec2 *)malloc((size_t)n * sizeof(*pts));
        CHECK(pts != NULL);
        for (int i = 0; i < n; i++) {
            float a = 6.2831853f * (float)i / (float)n;
            pts[i] = tmb_v2(64.0f + 60.0f * cosf(a), 64.0f + 60.0f * sinf(a));
        }
        /* into a buffer of exactly n (the documented capacity) */
        tmb_vec2 *out = (tmb_vec2 *)malloc((size_t)n * sizeof(*out));
        CHECK(out != NULL);
        int m = tmb_convex_hull(pts, n, out);
        CHECK(m >= 3);
        CHECK(m <= n);
        free(out);

        int cnt = -1;
        tmb_vec2 *h = tmb_convex_hull_alloc(pts, n, &cnt);
        CHECK(h != NULL);
        CHECK(cnt == m);
        free(h);
        free(pts);
    }
    return 0;
}

/* A square with interior points: hull must be the 4 corners. */
static int test_square_with_interior(void)
{
    tmb_vec2 pts[] = {
        {0, 0}, {10, 0}, {10, 10}, {0, 10}, /* corners */
        {5, 5}, {3, 7}, {8, 2}, {6, 6},     /* interior */
    };
    int n = (int)(sizeof(pts) / sizeof(pts[0]));
    int cnt = -1;
    tmb_vec2 *h = tmb_convex_hull_alloc(pts, n, &cnt);
    CHECK(h != NULL);
    CHECK(cnt == 4);
    free(h);
    return 0;
}

/* Collinear points collapse to the two extreme points. */
static int test_collinear(void)
{
    tmb_vec2 pts[] = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};
    int n = (int)(sizeof(pts) / sizeof(pts[0]));
    tmb_vec2 out[8];
    int m = tmb_convex_hull(pts, n, out);
    CHECK(m <= 2);
    return 0;
}

/* Duplicate points must not inflate the hull or overrun anything. */
static int test_duplicates(void)
{
    tmb_vec2 pts[16];
    for (int i = 0; i < 16; i++)
        pts[i] = tmb_v2(5.0f, 5.0f); /* all identical */
    int cnt = -1;
    tmb_vec2 *h = tmb_convex_hull_alloc(pts, 16, &cnt);
    CHECK(h != NULL);
    CHECK(cnt <= 1);
    free(h);

    /* a triangle with each vertex duplicated */
    tmb_vec2 tri[] = {{0, 0}, {0, 0}, {4, 0}, {4, 0}, {2, 3}, {2, 3}};
    cnt = -1;
    h = tmb_convex_hull_alloc(tri, 6, &cnt);
    CHECK(h != NULL);
    CHECK(cnt == 3);
    free(h);
    return 0;
}

/* Degenerate counts. */
static int test_small_inputs(void)
{
    tmb_vec2 one[] = {{1, 1}};
    tmb_vec2 two[] = {{1, 1}, {2, 2}};
    tmb_vec2 out[4];
    CHECK(tmb_convex_hull(one, 1, out) == 1);
    CHECK(tmb_convex_hull(two, 2, out) == 2);

    int cnt = -1;
    CHECK(tmb_convex_hull_alloc(NULL, 0, &cnt) == NULL);
    CHECK(cnt == 0);
    return 0;
}

/* tmb_hull_monotone honours a tight cap without writing past it. */
static int test_cap_respected(void)
{
    tmb_vec2 pts[8] = {{0, 0}, {4, 0}, {4, 4}, {0, 4},
                       {1, 1}, {2, 1}, {3, 2}, {1, 3}};
    tmb_vec2 out[2];
    int m = tmb_hull_monotone(pts, 8, out, 2);
    CHECK(m == 2); /* clamped to cap */
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_convex_position_no_overflow();
    r |= test_square_with_interior();
    r |= test_collinear();
    r |= test_duplicates();
    r |= test_small_inputs();
    r |= test_cap_respected();
    TT_DONE();
}
