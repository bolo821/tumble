#include "tumble/polygon.h"

#include <stdlib.h>

float tmb_poly_signed_area(const tmb_vec2 *v, int n)
{
    float a = 0.0f;
    for (int i = 0; i < n; i++)
        a += tmb_v2_cross(v[i], v[(i + 1) % n]);
    return 0.5f * a;
}

float tmb_poly_perimeter(const tmb_vec2 *v, int n)
{
    float p = 0.0f;
    for (int i = 0; i < n; i++)
        p += tmb_v2_dist(v[i], v[(i + 1) % n]);
    return p;
}

tmb_vec2 tmb_poly_centroid(const tmb_vec2 *v, int n)
{
    tmb_vec2 c = tmb_v2(0, 0);
    float a = 0.0f;
    for (int i = 0; i < n; i++) {
        tmb_vec2 p0 = v[i], p1 = v[(i + 1) % n];
        float cr = tmb_v2_cross(p0, p1);
        a += cr;
        c = tmb_v2_add(c, tmb_v2_scale(tmb_v2_add(p0, p1), cr));
    }
    if (a == 0.0f)
        return n ? v[0] : tmb_v2(0, 0);
    return tmb_v2_scale(c, 1.0f / (3.0f * a));
}

int tmb_poly_is_convex(const tmb_vec2 *v, int n)
{
    if (n < 3)
        return 0;
    int sign = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 a = tmb_v2_sub(v[(i + 1) % n], v[i]);
        tmb_vec2 b = tmb_v2_sub(v[(i + 2) % n], v[(i + 1) % n]);
        float cr = tmb_v2_cross(a, b);
        int s = (cr > 0) - (cr < 0);
        if (s != 0) {
            if (sign == 0)
                sign = s;
            else if (s != sign)
                return 0;
        }
    }
    return 1;
}

int tmb_poly_contains(const tmb_vec2 *v, int n, tmb_vec2 p)
{
    int inside = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((v[i].y > p.y) != (v[j].y > p.y)) &&
            (p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) +
                       v[i].x))
            inside = !inside;
    }
    return inside;
}

int tmb_poly_clip(const tmb_vec2 *v, int n, tmb_vec2 normal, float offset,
                  tmb_vec2 *out)
{
    int m = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 cur = v[i];
        tmb_vec2 nxt = v[(i + 1) % n];
        float dc = tmb_v2_dot(normal, cur) - offset;
        float dn = tmb_v2_dot(normal, nxt) - offset;
        if (dc <= 0.0f)
            out[m++] = cur;
        if ((dc < 0.0f) != (dn < 0.0f)) {
            float t = dc / (dc - dn);
            out[m++] = tmb_v2_lerp(cur, nxt, t);
        }
    }
    return m;
}

static int cmp_v2(const void *pa, const void *pb)
{
    const tmb_vec2 *a = (const tmb_vec2 *)pa, *b = (const tmb_vec2 *)pb;
    if (a->x < b->x) return -1;
    if (a->x > b->x) return 1;
    if (a->y < b->y) return -1;
    if (a->y > b->y) return 1;
    return 0;
}

int tmb_convex_hull(const tmb_vec2 *pts, int n, tmb_vec2 *hull)
{
    if (n < 3) {
        for (int i = 0; i < n; i++)
            hull[i] = pts[i];
        return n;
    }
    tmb_vec2 *s = (tmb_vec2 *)malloc((size_t)n * sizeof(*s));
    if (!s)
        return 0;
    for (int i = 0; i < n; i++)
        s[i] = pts[i];
    qsort(s, (size_t)n, sizeof(*s), cmp_v2);

    int k = 0;
    for (int i = 0; i < n; i++) {
        while (k >= 2 &&
               tmb_v2_cross(tmb_v2_sub(hull[k - 1], hull[k - 2]),
                            tmb_v2_sub(s[i], hull[k - 2])) <= 0)
            k--;
        hull[k++] = s[i];
    }
    int lower = k + 1;
    for (int i = n - 2; i >= 0; i--) {
        while (k >= lower &&
               tmb_v2_cross(tmb_v2_sub(hull[k - 1], hull[k - 2]),
                            tmb_v2_sub(s[i], hull[k - 2])) <= 0)
            k--;
        hull[k++] = s[i];
    }
    free(s);
    return k > 1 ? k - 1 : k;
}

/* Convex hull of a point cloud, returned in a freshly allocated array that the
 * caller owns and frees. *out_count receives the number of hull vertices.
 * A convenience wrapper around tmb_convex_hull for callers that would rather
 * not size a scratch buffer themselves. */
tmb_vec2 *tmb_convex_hull_alloc(const tmb_vec2 *pts, int n, int *out_count)
{
    if (out_count)
        *out_count = 0;
    if (n < 1)
        return NULL;
    /* A convex hull has no more vertices than the input point set, so size the
     * output to the point count. */
    tmb_vec2 *hull = (tmb_vec2 *)malloc((size_t)n * sizeof(*hull));
    if (!hull)
        return NULL;
    int m = tmb_convex_hull(pts, n, hull);
    if (out_count)
        *out_count = m;
    return hull;
}
