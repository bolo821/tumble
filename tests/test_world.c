#include "tumble/tumble.h"
#include "tumble/builder.h"
#include "tttest.h"

int main(void)
{
    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -10);

    tmb_build_ground(&w, 20, -5);
    uint32_t ball = tmb_build_ball(&w, tmb_v2(0, 5), 0.5f);
    CHECK(tmb_world_body_count(&w) == 2);

    float y0 = tmb_world_body(&w, ball)->pos.y;
    for (int i = 0; i < 120; i++)
        tmb_world_step(&w, 1.0f / 60.0f);
    float y1 = tmb_world_body(&w, ball)->pos.y;
    CHECK(y1 < y0);            /* it fell */
    CHECK(y1 > -6.0f);         /* and was stopped by the floor */
    tmb_world_free(&w);
    TT_DONE();
}
