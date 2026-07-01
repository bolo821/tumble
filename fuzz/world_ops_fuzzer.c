/* Fuzz target: drive the world through a stream of lifecycle operations.
 *
 * The scene loaders describe a static world and then step it. This target
 * instead interprets the input as a little command stream -- add bodies, step,
 * evict a body from the largest island, nudge a body -- so the fuzzer can
 * explore the *stateful* behaviour of the engine over many steps: islands
 * forming, going to sleep, being disturbed and waking, bodies being evicted
 * from settled configurations between those events.
 *
 * Sleeping is enabled so the sleep/wake machinery is exercised. */

#include "tumble/tumble.h"
#include "tumble/island_sleep.h"
#include "tumble/warmstart.h"
#include "tumble/joints_ext.h"
#include "tumble/contact_events.h"
#include "tumble/recorder.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* A cursor over the input bytes with saturating reads (0 past the end). */
typedef struct {
    const uint8_t *p;
    size_t n, i;
} cursor;

static uint8_t u8(cursor *c)
{
    return c->i < c->n ? c->p[c->i++] : 0;
}

/* A small signed coordinate in roughly [-4, 4]. */
static float coord(cursor *c)
{
    return (float)((int)u8(c) - 128) / 32.0f;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -10);
    w.enable_sleeping = 1;

    tmb_ws_cache *ws = (tmb_ws_cache *)calloc(1, sizeof(*ws));
    tmb_ws_cache_init(ws);
    w.warmstart = ws;

    tmb_contact_tracker *tracker =
        (tmb_contact_tracker *)malloc(sizeof(*tracker));
    tmb_contact_tracker_init(tracker);
    w.contact_tracker = (struct tmb_contact_tracker *)tracker;

    /* State recorder: snapshot body kinematics each step so replay and
     * divergence checks are available after a run completes. */
    tmb_recorder rec;
    tmb_recorder_init(&rec, 4);

    /* a floor so dropped bodies pile up and settle into islands */
    tmb_body floor;
    tmb_body_init(&floor, TMB_BODY_STATIC, tmb_shape_box(40, 1), tmb_v2(0, -2));
    tmb_world_add_body(&w, &floor);

    cursor c = {data, size, 0};
    int budget = 6000; /* bound total work regardless of input size */

    while (c.i < c.n && budget-- > 0) {
        uint8_t op = u8(&c);
        switch (op % 8) {
        case 0: { /* add a small dynamic box, biased to a tight column */
            float x = coord(&c) * 0.5f;
            float y = -0.5f + (float)(u8(&c) % 20) * 0.6f;
            tmb_body b;
            tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(0.5f, 0.5f),
                          tmb_v2(x, y));
            tmb_world_add_body(&w, &b);
            break;
        }
        case 1: { /* add a dynamic circle */
            float x = coord(&c) * 0.5f;
            float y = -0.5f + (float)(u8(&c) % 20) * 0.6f;
            tmb_body b;
            tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_circle(0.45f),
                          tmb_v2(x, y));
            tmb_world_add_body(&w, &b);
            break;
        }
        case 2: { /* advance the simulation */
            int steps = 1 + (u8(&c) % 40);
            for (int k = 0; k < steps && budget-- > 0; k++) {
                tmb_world_step(&w, 1.0f / 60.0f);
                tmb_recorder_capture(&rec, &w);
            }
            break;
        }
        case 3: { /* remove a body from the scene by index */
            /* Models a scene-management event that removes a body to free its
             * slot for a new actor; the body index is chosen from live bodies. */
            size_t bc = tmb_world_body_count(&w);
            if (bc > 1) {
                uint8_t slot = u8(&c);
                uint32_t idx = 1 + (uint32_t)(slot % (uint32_t)(bc - 1));
                tmb_world_remove_body(&w, idx);
            } else {
                (void)u8(&c);
            }
            break;
        }
        case 4: { /* nudge a body to disturb a resting island */
            size_t bc = tmb_world_body_count(&w);
            if (bc > 1) {
                uint32_t idx = 1 + (u8(&c) % (uint32_t)(bc - 1));
                tmb_body *b = tmb_world_body(&w, idx);
                b->vel = tmb_v2(coord(&c), 4.0f + (float)(u8(&c) % 8));
            } else {
                (void)u8(&c);
                (void)u8(&c);
            }
            break;
        }
        case 5: { /* drop a heavier box from above to land on the pile */
            float x = coord(&c) * 0.5f;
            tmb_body b;
            tmb_body_init(&b, TMB_BODY_DYNAMIC, tmb_shape_box(0.6f, 0.6f),
                          tmb_v2(x, 4.0f));
            tmb_world_add_body(&w, &b);
            break;
        }
        case 6: { /* add a distance joint between two dynamic bodies */
            size_t bc = tmb_world_body_count(&w);
            if (bc > 2) {
                uint32_t ia = 1 + (u8(&c) % (uint32_t)(bc - 1));
                uint32_t ib = 1 + (u8(&c) % (uint32_t)(bc - 1));
                if (ia != ib) {
                    tmb_joint j;
                    memset(&j, 0, sizeof(j));
                    j.kind = TMB_JOINT_DISTANCE;
                    j.a = ia;
                    j.b = ib;
                    j.rest_length = 1.0f;
                    tmb_world_add_joint(&w, &j);
                }
            } else {
                (void)u8(&c);
                (void)u8(&c);
            }
            break;
        }
        case 7: { /* add a weld xjoint between two dynamic bodies */
            size_t bc = tmb_world_body_count(&w);
            if (bc > 2) {
                uint32_t ia = 1 + (u8(&c) % (uint32_t)(bc - 1));
                uint32_t ib = 1 + (u8(&c) % (uint32_t)(bc - 1));
                if (ia != ib) {
                    tmb_xjoint xj;
                    tmb_xjoint_weld(&xj, ia, ib);
                    tmb_world_add_xjoint(&w, &xj);
                }
            } else {
                (void)u8(&c);
                (void)u8(&c);
            }
            break;
        }
        }
    }

    w.contact_tracker = NULL;
    tmb_contact_tracker_free(tracker);
    free(tracker);

    tmb_recorder_free(&rec);
    tmb_world_free(&w);
    return 0;
}
