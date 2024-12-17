/* Fuzz target: load a binary scene and simulate it.
 *
 * The binary format is more compact than the text format, so the fuzzer can
 * describe large scenes with many bodies in few bytes -- useful for reaching
 * the dense-contact code paths. The harness loads, steps and tears down. */

#include "tumble/tumble.h"

#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    tmb_world w;
    tmb_world_init(&w);

    if (tmb_load_scene_bin(data, size, &w) != TMB_OK) {
        tmb_world_free(&w);
        return 0;
    }

    for (int i = 0; i < 300; i++)
        tmb_world_step(&w, 1.0f / 60.0f);

    tmb_world_free(&w);
    return 0;
}
