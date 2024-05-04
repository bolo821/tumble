#include "tumble/hull.h"

static float cross_o(tmb_vec2 o, tmb_vec2 a, tmb_vec2 b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

/* sort indices by (x, then y) without allocating: simple insertion sort over a
 * caller-sized scratch is overkill, so we sort a local copy of the points */
static void sort_points(tmb_vec2 *p, int n)
{
    for (int i = 1; i < n; i++) {
        tmb_vec2 key = p[i];
        int j = i - 1;
        while (j >= 0 && (p[j].x > key.x ||
                          (p[j].x == key.x && p[j].y > key.y))) {
            p[j + 1] = p[j];
            j--;
        }
        p[j + 1] = key;
    }
}

int tmb_convex_hull_capped(const tmb_vec2 *pts, int n, tmb_vec2 *out, int cap)
{
    if (n < 3) {
        int m = n < cap ? n : cap;
        for (int i = 0; i < m; i++)
            out[i] = pts[i];
        return m;
    }
    tmb_vec2 sorted[256];
    if (n > 256)
        n = 256;
    for (int i = 0; i < n; i++)
        sorted[i] = pts[i];
    sort_points(sorted, n);

    tmb_vec2 hull[512];
    int k = 0;
    /* lower hull */
    for (int i = 0; i < n; i++) {
        while (k >= 2 && cross_o(hull[k - 2], hull[k - 1], sorted[i]) <= 0)
            k--;
        hull[k++] = sorted[i];
    }
    /* upper hull */
    int lower = k + 1;
    for (int i = n - 2; i >= 0; i--) {
        while (k >= lower && cross_o(hull[k - 2], hull[k - 1], sorted[i]) <= 0)
            k--;
        hull[k++] = sorted[i];
    }
    k--; /* last point equals the first */
    int m = k < cap ? k : cap;
    for (int i = 0; i < m; i++)
        out[i] = hull[i];
    return m;
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
