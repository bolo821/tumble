#include "tumble/hull.h"

#include <stdlib.h>

static float cross_o(tmb_vec2 o, tmb_vec2 a, tmb_vec2 b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

static int cmp_pt(const void *pa, const void *pb)
{
    const tmb_vec2 *a = (const tmb_vec2 *)pa, *b = (const tmb_vec2 *)pb;
    if (a->x < b->x) return -1;
    if (a->x > b->x) return 1;
    if (a->y < b->y) return -1;
    if (a->y > b->y) return 1;
    return 0;
}

/* Single, bounds-safe monotone-chain hull core shared by the hull utilities
 * and the polygon helpers.
 *
 * Andrew's monotone chain writes each chain into a working array whose length
 * can reach the input count plus the closing vertex; a caller that sized its
 * destination to exactly the point count (a natural assumption, since the hull
 * never has *more* vertices than the input) would be overrun by that closing
 * write. To make the routine safe for any destination size, the chain is built
 * in an internal scratch buffer and only the first min(count, cap) vertices are
 * copied out. Coincident points are removed first so degenerate clouds cannot
 * inflate the vertex count. Returns the number of vertices written to `out`
 * (never more than `cap`). */
int tmb_hull_monotone(const tmb_vec2 *pts, int n, tmb_vec2 *out, int cap)
{
    if (n <= 0 || cap <= 0 || !out)
        return 0;

    tmb_vec2 *s = (tmb_vec2 *)malloc((size_t)n * sizeof(*s));
    if (!s)
        return 0;
    for (int i = 0; i < n; i++)
        s[i] = pts[i];
    qsort(s, (size_t)n, sizeof(*s), cmp_pt);

    /* Drop coincident points (the array is sorted, so duplicates are adjacent). */
    int m = 0;
    for (int i = 0; i < n; i++)
        if (m == 0 || s[i].x != s[m - 1].x || s[i].y != s[m - 1].y)
            s[m++] = s[i];

    int written;
    if (m < 3) {
        written = m < cap ? m : cap;
        for (int i = 0; i < written; i++)
            out[i] = s[i];
        free(s);
        return written;
    }

    /* Worst case the chain holds 2*m points before the closing vertex is
     * dropped; +1 keeps a safe margin. */
    tmb_vec2 *h = (tmb_vec2 *)malloc((size_t)(2 * m + 1) * sizeof(*h));
    if (!h) {
        free(s);
        return 0;
    }

    int k = 0;
    for (int i = 0; i < m; i++) { /* lower hull */
        while (k >= 2 && cross_o(h[k - 2], h[k - 1], s[i]) <= 0)
            k--;
        h[k++] = s[i];
    }
    int lower = k + 1;
    for (int i = m - 2; i >= 0; i--) { /* upper hull */
        while (k >= lower && cross_o(h[k - 2], h[k - 1], s[i]) <= 0)
            k--;
        h[k++] = s[i];
    }
    int count = k > 0 ? k - 1 : 0; /* last point repeats the first */

    written = count < cap ? count : cap;
    for (int i = 0; i < written; i++)
        out[i] = h[i];

    free(h);
    free(s);
    return written;
}

int tmb_convex_hull_capped(const tmb_vec2 *pts, int n, tmb_vec2 *out, int cap)
{
    return tmb_hull_monotone(pts, n, out, cap);
}

float tmb_polygon_area(const tmb_vec2 *pts, int n)
{
    float a = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 p0 = pts[i];
        tmb_vec2 p1 = pts[(i + 1) % n];
        a += p0.x * p1.y - p1.x * p0.y;
    }
    return a * 0.5f;
}

tmb_vec2 tmb_polygon_centroid(const tmb_vec2 *pts, int n)
{
    tmb_vec2 c = {0, 0};
    float a = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 p0 = pts[i];
        tmb_vec2 p1 = pts[(i + 1) % n];
        float cr = p0.x * p1.y - p1.x * p0.y;
        a += cr;
        c.x += (p0.x + p1.x) * cr;
        c.y += (p0.y + p1.y) * cr;
    }
    if (a > -1e-12f && a < 1e-12f)
        return pts[0];
    float inv = 1.0f / (3.0f * a);
    c.x *= inv;
    c.y *= inv;
    return c;
}

int tmb_polygon_is_convex(const tmb_vec2 *pts, int n)
{
    if (n < 3)
        return 0;
    int sign = 0;
    for (int i = 0; i < n; i++) {
        tmb_vec2 a = pts[i];
        tmb_vec2 b = pts[(i + 1) % n];
        tmb_vec2 c = pts[(i + 2) % n];
        float cr = cross_o(a, b, c);
        int s = cr > 0 ? 1 : (cr < 0 ? -1 : 0);
        if (s != 0) {
            if (sign == 0)
                sign = s;
            else if (sign != s)
                return 0;
        }
    }
    return 1;
}

int tmb_polygon_contains(const tmb_vec2 *pts, int n, tmb_vec2 p)
{
    int inside = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((pts[i].y > p.y) != (pts[j].y > p.y)) &&
            (p.x < (pts[j].x - pts[i].x) * (p.y - pts[i].y) /
                           (pts[j].y - pts[i].y) +
                       pts[i].x))
            inside = !inside;
    }
    return inside;
}

void tmb_polygon_reverse(tmb_vec2 *pts, int n)
{
    for (int i = 0, j = n - 1; i < j; i++, j--) {
        tmb_vec2 t = pts[i];
        pts[i] = pts[j];
        pts[j] = t;
    }
}
