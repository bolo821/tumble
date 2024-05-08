#include "tumble/triangulate.h"

#include <math.h>

static float area2(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static int point_in_tri(tmb_vec2 p, tmb_vec2 a, tmb_vec2 b, tmb_vec2 c)
{
    float d1 = area2(p, a, b);
    float d2 = area2(p, b, c);
    float d3 = area2(p, c, a);
    int neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    int pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(neg && pos);
}

int tmb_triangulate(const tmb_vec2 *pts, int n, tmb_tri *out, int cap)
{
    if (n < 3)
        return 0;
    int idx[256];
    if (n > 256)
        return 0;

    /* establish winding: ear clipping below assumes counter-clockwise */
    float sum = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 p0 = pts[i];
        tmb_vec2 p1 = pts[(i + 1) % n];
        sum += p0.x * p1.y - p1.x * p0.y;
    }
    if (sum < 0)
        for (int i = 0; i < n; i++)
            idx[i] = n - 1 - i;
    else
        for (int i = 0; i < n; i++)
            idx[i] = i;

    int remaining = n;
    int count = 0;
    int guard = 0;
    int i = 0;
    while (remaining > 3 && guard < 4 * n) {
        guard++;
        int i0 = idx[(i + remaining - 1) % remaining];
        int i1 = idx[i % remaining];
        int i2 = idx[(i + 1) % remaining];
        tmb_vec2 a = pts[i0], b = pts[i1], c = pts[i2];
        if (area2(a, b, c) > 1e-9f) {
            int is_ear = 1;
            for (int k = 0; k < remaining; k++) {
                int vi = idx[k];
                if (vi == i0 || vi == i1 || vi == i2)
                    continue;
                if (point_in_tri(pts[vi], a, b, c)) {
                    is_ear = 0;
                    break;
                }
            }
            if (is_ear) {
                if (count < cap) {
                    out[count].a = i0;
                    out[count].b = i1;
                    out[count].c = i2;
                }
                count++;
                /* remove i1 */
                for (int k = i % remaining; k < remaining - 1; k++)
                    idx[k] = idx[k + 1];
                remaining--;
                guard = 0;
                continue;
            }
        }
        i++;
    }
    if (remaining == 3) {
        if (count < cap) {
            out[count].a = idx[0];
            out[count].b = idx[1];
            out[count].c = idx[2];
        }
        count++;
    }
    return count;
}

float tmb_triangulated_area(const tmb_vec2 *pts, int n, const tmb_tri *tris,
                            int count)
{
    (void)n;
    float total = 0;
    for (int i = 0; i < count; i++) {
        float a = area2(pts[tris[i].a], pts[tris[i].b], pts[tris[i].c]);
        total += (a < 0 ? -a : a) * 0.5f;
    }
    return total;
}
