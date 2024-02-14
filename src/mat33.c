#include "tumble/mat33.h"

#include <math.h>

tmb_mat33 tmb_mat33_identity(void)
{
    tmb_mat33 m = {{1, 0, 0, 0, 1, 0, 0, 0, 1}};
    return m;
}

tmb_mat33 tmb_mat33_translation(float x, float y)
{
    tmb_mat33 m = tmb_mat33_identity();
    m.m[2] = x;
    m.m[5] = y;
    return m;
}

tmb_mat33 tmb_mat33_rotation(float r)
{
    float c = cosf(r), s = sinf(r);
    tmb_mat33 m = {{c, -s, 0, s, c, 0, 0, 0, 1}};
    return m;
}

tmb_mat33 tmb_mat33_scale(float sx, float sy)
{
    tmb_mat33 m = {{sx, 0, 0, 0, sy, 0, 0, 0, 1}};
    return m;
}

tmb_mat33 tmb_mat33_mul(tmb_mat33 a, tmb_mat33 b)
{
    tmb_mat33 r;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            float s = 0;
            for (int k = 0; k < 3; k++)
                s += a.m[i * 3 + k] * b.m[k * 3 + j];
            r.m[i * 3 + j] = s;
        }
    return r;
}

tmb_vec3 tmb_mat33_apply(tmb_mat33 m, tmb_vec3 v)
{
    tmb_vec3 r;
    r.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z;
    r.y = m.m[3] * v.x + m.m[4] * v.y + m.m[5] * v.z;
    r.z = m.m[6] * v.x + m.m[7] * v.y + m.m[8] * v.z;
    return r;
}

tmb_vec2 tmb_mat33_point(tmb_mat33 m, tmb_vec2 p)
{
    tmb_vec3 v = {p.x, p.y, 1.0f};
    tmb_vec3 r = tmb_mat33_apply(m, v);
    return tmb_v2(r.x, r.y);
}

tmb_vec2 tmb_mat33_dir(tmb_mat33 m, tmb_vec2 d)
{
    tmb_vec3 v = {d.x, d.y, 0.0f};
    tmb_vec3 r = tmb_mat33_apply(m, v);
    return tmb_v2(r.x, r.y);
}

float tmb_mat33_det(tmb_mat33 m)
{
    return m.m[0] * (m.m[4] * m.m[8] - m.m[5] * m.m[7]) -
           m.m[1] * (m.m[3] * m.m[8] - m.m[5] * m.m[6]) +
           m.m[2] * (m.m[3] * m.m[7] - m.m[4] * m.m[6]);
}

tmb_vec3 tmb_mat33_solve(tmb_mat33 m, tmb_vec3 rhs)
{
    float det = tmb_mat33_det(m);
    tmb_vec3 z = {0, 0, 0};
    if (det > -1e-20f && det < 1e-20f)
        return z;
    float inv = 1.0f / det;
    /* Cramer's rule */
    tmb_mat33 mx = m, my = m, mz = m;
    mx.m[0] = rhs.x;
    mx.m[3] = rhs.y;
    mx.m[6] = rhs.z;
    my.m[1] = rhs.x;
    my.m[4] = rhs.y;
    my.m[7] = rhs.z;
    mz.m[2] = rhs.x;
    mz.m[5] = rhs.y;
    mz.m[8] = rhs.z;
    tmb_vec3 r;
    r.x = tmb_mat33_det(mx) * inv;
    r.y = tmb_mat33_det(my) * inv;
    r.z = tmb_mat33_det(mz) * inv;
    return r;
}
