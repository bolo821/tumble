#include "tumble/geom.h"
#include "tumble/hull.h"
#include "tumble/segment.h"
#include "tttest.h"

int main(void)
{
    /* hull of a square-ish point cloud */
    tmb_vec2 pts[6] = {{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 1}, {1, 0.5f}};
    tmb_vec2 hull[8];
    int n = tmb_convex_hull_capped(pts, 6, hull, 8);
    CHECK(n == 4);
    CHECK(tmb_polygon_is_convex(hull, n));

    /* segment intersection */
    tmb_segment s1 = {{-1, 0}, {1, 0}};
    tmb_segment s2 = {{0, -1}, {0, 1}};
    tmb_vec2 hit;
    CHECK(tmb_seg_intersect(s1, s2, &hit, NULL, NULL) == 1);
    CHECK_NEAR(hit.x, 0.0f, 1e-5);
    CHECK_NEAR(hit.y, 0.0f, 1e-5);

    /* orientation and barycentric */
    CHECK(tmb_orient2d(tmb_v2(0, 0), tmb_v2(1, 0), tmb_v2(0, 1)) > 0);
    float u, v, ww;
    tmb_barycentric(tmb_v2(0.25f, 0.25f), tmb_v2(0, 0), tmb_v2(1, 0),
                    tmb_v2(0, 1), &u, &v, &ww);
    CHECK_NEAR(u + v + ww, 1.0f, 1e-5);

    tmb_vec2 ctr;
    float rad;
    CHECK(tmb_circumcircle(tmb_v2(0, 0), tmb_v2(2, 0), tmb_v2(0, 2), &ctr,
                           &rad) == 0);
    CHECK_NEAR(ctr.x, 1.0f, 1e-4);
    TT_DONE();
}
