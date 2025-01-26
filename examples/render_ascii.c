/* Drop a few shapes and draw an ASCII frame. */
#include "tumble/tumble.h"
#include "tumble/ascii_view.h"
#include "tumble/builder.h"
#include <stdio.h>

int main(void)
{
    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -10);
    tmb_build_ground(&w, 18, -6);
    tmb_build_ball(&w, tmb_v2(-2, 2), 0.8f);
    tmb_build_box(&w, tmb_v2(2, 3), 0.7f, 0.7f);

    for (int i = 0; i < 120; i++)
        tmb_world_step(&w, 1.0f / 60.0f);

    tmb_buf out;
    tmb_buf_init(&out);
    if (tmb_render_ascii(&w, 50, 20, &out) == TMB_OK && out.data)
        fwrite(out.data, 1, out.len, stdout);
    tmb_buf_free(&out);
    tmb_world_free(&w);
    return 0;
}
