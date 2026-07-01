#include "tumble/transform.h"

#include <math.h>

tmb_xform tmb_xform_identity(void)
{
    tmb_xform xf;
    xf.pos = tmb_v2(0, 0);
    xf.s = 0.0f;
    xf.c = 1.0f;
    return xf;
}

tmb_xform tmb_xform_make(tmb_vec2 pos, float angle)
{
    tmb_xform xf;
    xf.pos = pos;
    xf.s = sinf(angle);
    xf.c = cosf(angle);
    return xf;
}

void tmb_xform_set_angle(tmb_xform *xf, float angle)
{
    xf->s = sinf(angle);
    xf->c = cosf(angle);
}

tmb_vec2 tmb_xform_dir(tmb_xform xf, tmb_vec2 d)
{
    return tmb_v2(xf.c * d.x - xf.s * d.y, xf.s * d.x + xf.c * d.y);
}

tmb_vec2 tmb_xform_point(tmb_xform xf, tmb_vec2 local)
{
    return tmb_v2_add(xf.pos, tmb_xform_dir(xf, local));
}

tmb_vec2 tmb_xform_unpoint(tmb_xform xf, tmb_vec2 world)
{
    tmb_vec2 d = tmb_v2_sub(world, xf.pos);
    /* inverse rotation: transpose of the rotation matrix */
    return tmb_v2(xf.c * d.x + xf.s * d.y, -xf.s * d.x + xf.c * d.y);
}

float tmb_clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

int tmb_floor_mod(long v, int m)
{
    /* Euclidean (floored) modulo: the result is always in [0, m) for m > 0,
     * unlike C's '%', which keeps the sign of the dividend and would yield a
     * negative index for a negative v. */
    if (m <= 0)
        return 0;
    long r = v % m;
    if (r < 0)
        r += m;
    return (int)r;
}

float tmb_absf(float v) { return v < 0 ? -v : v; }
float tmb_minf(float a, float b) { return a < b ? a : b; }
float tmb_maxf(float a, float b) { return a > b ? a : b; }
