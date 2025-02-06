#include "tumble/tumble.h"
#include "tttest.h"
#include <string.h>

int main(void)
{
    const char *text =
        "gravity 0 -9.8\niters 8 3\n"
        "box 0 -2 10 0.5 static\ncircle 0 4 0.5 dynamic\n";
    tmb_world w;
    tmb_world_init(&w);
    CHECK(tmb_load_scene_text(text, strlen(text), &w) == TMB_OK);
    CHECK(tmb_world_body_count(&w) == 2);
    CHECK_NEAR(w.gravity.y, -9.8f, 1e-4);

    /* round-trip through the binary format */
    tmb_buf bin;
    tmb_buf_init(&bin);
    CHECK(tmb_write_scene_bin(&w, &bin) == TMB_OK);
    CHECK(bin.len > 4 && memcmp(bin.data, "TMBB", 4) == 0);

    tmb_world w2;
    tmb_world_init(&w2);
    CHECK(tmb_load_scene_bin(bin.data, bin.len, &w2) == TMB_OK);
    CHECK(tmb_world_body_count(&w2) == tmb_world_body_count(&w));

    tmb_buf_free(&bin);
    tmb_world_free(&w);
    tmb_world_free(&w2);

    /* malformed input must not crash and must not over-allocate */
    tmb_world w3;
    tmb_world_init(&w3);
    const char junk[] = "box\nxyzzy 1 2 3\npoly 0 0 99 1 2\n";
    tmb_load_scene_text(junk, sizeof(junk) - 1, &w3);
    tmb_world_free(&w3);
    TT_DONE();
}
