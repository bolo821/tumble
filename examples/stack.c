/* Build a stack of boxes with the builder helpers and let it settle. */
#include "tumble/tumble.h"
#include "tumble/builder.h"
#include "tumble/stats.h"
#include <stdio.h>

int main(void)
{
    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -10);

    tmb_build_ground(&w, 20, -6);
    tmb_build_stack(&w, 0, -5.5f, 8, 1.0f);

    for (int i = 0; i < 240; i++)
        tmb_world_step(&w, 1.0f / 60.0f);

    tmb_stats s;
    tmb_world_stats(&w, &s);
    tmb_buf out;
    tmb_buf_init(&out);
    tmb_stats_print(&s, &out);
    if (out.data)
        fwrite(out.data, 1, out.len, stdout);
    tmb_buf_free(&out);
    tmb_world_free(&w);
    return 0;
}
