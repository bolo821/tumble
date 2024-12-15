/* Fuzz target: load a text scene and simulate it.
 *
 * The harness mirrors how an application drives Tumble: parse a scene into a
 * world, then advance it for a number of steps. Exercising the full
 * load -> broadphase -> narrowphase -> solve pipeline is what gives the fuzzer
 * reach into the collision and contact-buffer code. */

#include "tumble/tumble.h"

#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    tmb_world w;
    tmb_world_init(&w);

    if (tmb_load_scene_text((const char *)data, size, &w) != TMB_OK) {
        tmb_world_free(&w);
        return 0;
    }

    /* Run a fixed budget of steps so a scene has time to settle into the
     * resting contacts that stress the narrowphase. */
    for (int i = 0; i < 300; i++)
        tmb_world_step(&w, 1.0f / 60.0f);

    tmb_world_free(&w);
    return 0;
}
