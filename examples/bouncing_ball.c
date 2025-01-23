/* A ball dropped onto a static floor; prints its height each step. */
#include "tumble/tumble.h"
#include <stdio.h>

int main(void)
{
    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -9.8f);

    tmb_body floor;
    tmb_body_init(&floor, TMB_BODY_STATIC, tmb_shape_box(10, 0.5f),
                  tmb_v2(0, -5));
    tmb_world_add_body(&w, &floor);

    tmb_body ball;
    tmb_body_init(&ball, TMB_BODY_DYNAMIC, tmb_shape_circle(0.5f), tmb_v2(0, 5));
    ball.restitution = 0.7f;
    uint32_t id = tmb_world_add_body(&w, &ball);

    for (int i = 0; i < 180; i++) {
        tmb_world_step(&w, 1.0f / 60.0f);
        if (i % 20 == 0)
            printf("t=%5.2f  y=%.3f\n", i / 60.0f,
                   tmb_world_body(&w, id)->pos.y);
    }
    tmb_world_free(&w);
    return 0;
}
