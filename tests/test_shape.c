#include "tumble/shape.h"
#include "tttest.h"

int main(void)
{
    tmb_shape c = tmb_shape_circle(2.0f);
    float mass, inertia;
    tmb_vec2 cen;
    tmb_shape_compute_mass(&c, 1.0f, &mass, &inertia, &cen);
    CHECK(mass > 0);
    CHECK(inertia > 0);

    tmb_shape b = tmb_shape_box(1.0f, 1.0f);
    CHECK(b.kind == TMB_SHAPE_POLYGON);
    CHECK(b.count == 4);

    tmb_shape reg;
    CHECK(tmb_shape_regular(&reg, 6, 1.0f) == TMB_OK);
    CHECK(reg.count == 6);

    tmb_aabb box = tmb_shape_aabb(&b, tmb_xform_identity());
    CHECK_NEAR(box.min.x, -1.0f, 1e-5);
    CHECK_NEAR(box.max.y, 1.0f, 1e-5);
    TT_DONE();
}
