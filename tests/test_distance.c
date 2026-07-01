/* Regression tests for GJK distance and the geometry closest-point helpers.
 *
 * The distance query must handle the full 3-point simplex (formed when shapes
 * overlap or nearly touch) without reading past the simplex, and must return
 * sensible separations for disjoint shapes. All tests run under ASan/UBSan. */

#include "tumble/distance.h"
#include "tumble/geom.h"
#include "tumble/shape.h"
#include "tumble/transform.h"

#include "tttest.h"
#include <math.h>

static int approx(float a, float b, float eps) { return fabsf(a - b) < eps; }

static int test_segment_closest(void)
{
    tmb_vec2 a = tmb_v2(0, 0), b = tmb_v2(10, 0);
    float t;
    tmb_vec2 c = tmb_closest_point_on_segment(tmb_v2(5, 3), a, b, &t);
    CHECK(approx(c.x, 5, 1e-4f) && approx(c.y, 0, 1e-4f));
    CHECK(approx(t, 0.5f, 1e-4f));
    c = tmb_closest_point_on_segment(tmb_v2(-4, 1), a, b, &t);
    CHECK(approx(t, 0.0f, 1e-4f)); /* clamped to a */
    c = tmb_closest_point_on_segment(tmb_v2(99, 1), a, b, &t);
    CHECK(approx(t, 1.0f, 1e-4f)); /* clamped to b */
    (void)c;
    return 0;
}

static int test_triangle_closest(void)
{
    tmb_vec2 a = tmb_v2(0, 0), b = tmb_v2(4, 0), c = tmb_v2(0, 4);
    tmb_tri_region reg;

    tmb_vec2 p = tmb_closest_point_on_triangle(tmb_v2(1, 1), a, b, c, &reg);
    CHECK(reg == TMB_TRI_INSIDE);
    CHECK(approx(p.x, 1, 1e-4f) && approx(p.y, 1, 1e-4f));

    p = tmb_closest_point_on_triangle(tmb_v2(-3, -3), a, b, c, &reg);
    CHECK(reg == TMB_TRI_VERT_A);

    p = tmb_closest_point_on_triangle(tmb_v2(2, -2), a, b, c, &reg);
    CHECK(reg == TMB_TRI_EDGE_AB);
    CHECK(approx(p.y, 0, 1e-4f));
    (void)p;
    return 0;
}

static float box_gap(float halfw, float sep)
{
    /* two axis-aligned unit boxes separated along x by `sep` centre-to-centre */
    tmb_shape a = tmb_shape_box(halfw, halfw);
    tmb_shape b = tmb_shape_box(halfw, halfw);
    tmb_xform xa = tmb_xform_make(tmb_v2(0, 0), 0.0f);
    tmb_xform xb = tmb_xform_make(tmb_v2(sep, 0), 0.0f);
    tmb_vec2 pa, pb;
    return tmb_shape_distance(&a, xa, &b, xb, &pa, &pb);
}

static int test_distance_separated(void)
{
    /* halfwidth 0.5, centres 3 apart -> gap 3 - 0.5 - 0.5 = 2 */
    CHECK(approx(box_gap(0.5f, 3.0f), 2.0f, 1e-3f));
    /* centres 1.5 apart -> gap 0.5 */
    CHECK(approx(box_gap(0.5f, 1.5f), 0.5f, 1e-3f));
    return 0;
}

static int test_distance_overlap(void)
{
    /* halfwidth 1.0, centres 0.5 apart -> overlapping -> distance 0 */
    CHECK(approx(box_gap(1.0f, 0.5f), 0.0f, 1e-4f));
    /* coincident */
    CHECK(approx(box_gap(1.0f, 0.0f), 0.0f, 1e-4f));
    return 0;
}

/* Sweep offsets across the overlap/separation boundary: must never crash and
 * distance must be monotone-ish (0 while overlapping, growing once apart). */
static int test_distance_sweep_no_oob(void)
{
    for (int i = 0; i <= 40; i++) {
        float sep = (float)i * 0.1f; /* 0 .. 4 */
        float d = box_gap(0.5f, sep);
        CHECK(d >= -1e-4f);
        if (sep <= 1.0f)
            CHECK(approx(d, 0.0f, 1e-3f)); /* boxes overlap up to sep=1 */
    }
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_segment_closest();
    r |= test_triangle_closest();
    r |= test_distance_separated();
    r |= test_distance_overlap();
    r |= test_distance_sweep_no_oob();
    TT_DONE();
}
