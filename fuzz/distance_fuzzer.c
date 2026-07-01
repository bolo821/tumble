/* Fuzz target: GJK distance between two convex polygons.
 *
 * Two convex shapes are built from the input and placed near each other, then
 * tmb_shape_distance runs GJK to find their separation (0 when they overlap).
 * Overlapping placements drive GJK to a full 3-point simplex, exercising the
 * triangle-reduction branch.
 *
 * Bytes are mapped to small coordinates/offsets, so any nonzero byte yields a
 * valid configuration. */

#include "tumble/tumble.h"
#include "tumble/distance.h"
#include "tumble/shape.h"
#include "tumble/transform.h"

#include <stdint.h>
#include <stddef.h>

static float b2f(uint8_t v, float lo, float hi)
{
    return lo + (hi - lo) * ((float)v / 255.0f);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t d[8] = {64, 64, 64, 64, 64, 64, 64, 64};
    for (size_t i = 0; i < size && i < 8; i++)
        d[i] = data[i];

    /* Two boxes, sized and offset from the input. The offset is kept small so
     * the two shapes usually overlap and GJK builds a full simplex. */
    tmb_shape a = tmb_shape_box(b2f(d[0], 0.5f, 2.0f), b2f(d[1], 0.5f, 2.0f));
    tmb_shape b = tmb_shape_box(b2f(d[2], 0.5f, 2.0f), b2f(d[3], 0.5f, 2.0f));

    tmb_xform xa = tmb_xform_make(tmb_v2(0.0f, 0.0f), b2f(d[6], -1.0f, 1.0f));
    tmb_xform xb = tmb_xform_make(
        tmb_v2(b2f(d[4], -1.5f, 1.5f), b2f(d[5], -1.5f, 1.5f)),
        b2f(d[7], -1.0f, 1.0f));

    tmb_vec2 pa, pb;
    (void)tmb_shape_distance(&a, xa, &b, xb, &pa, &pb);
    return 0;
}
