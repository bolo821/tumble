#include "tumble/vec2.h"
#include "tumble/mat22.h"
#include "tttest.h"

int main(void)
{
    tmb_vec2 a = tmb_v2(3, 4);
    CHECK_NEAR(tmb_v2_len(a), 5.0f, 1e-5);
    tmb_vec2 n = tmb_v2_normalize(a);
    CHECK_NEAR(tmb_v2_len(n), 1.0f, 1e-5);
    CHECK_NEAR(tmb_v2_dot(tmb_v2(1, 0), tmb_v2(0, 1)), 0.0f, 1e-6);
    CHECK_NEAR(tmb_v2_cross(tmb_v2(1, 0), tmb_v2(0, 1)), 1.0f, 1e-6);

    tmb_vec2 r = tmb_v2_rotate(tmb_v2(1, 0), 3.14159265f / 2.0f);
    CHECK_NEAR(r.x, 0.0f, 1e-4);
    CHECK_NEAR(r.y, 1.0f, 1e-4);

    tmb_mat22 m = tmb_mat22_rotation(3.14159265f / 2.0f);
    tmb_vec2 mr = tmb_mat22_apply(m, tmb_v2(1, 0));
    CHECK_NEAR(mr.y, 1.0f, 1e-4);
    int ok;
    tmb_mat22 inv = tmb_mat22_inverse(m, &ok);
    CHECK(ok == 1);
    tmb_vec2 back = tmb_mat22_apply(inv, mr);
    CHECK_NEAR(back.x, 1.0f, 1e-4);
    TT_DONE();
}
