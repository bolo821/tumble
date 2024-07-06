#include "tumble/pick.h"

size_t tmb_pick_radius(tmb_world *w, tmb_vec2 p, float radius, uint32_t *out,
                       size_t cap)
{
    float r2 = radius * radius;
    size_t n = tmb_world_body_count(w);
    size_t found = 0;
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        float dx = b->pos.x - p.x;
        float dy = b->pos.y - p.y;
        if (dx * dx + dy * dy <= r2) {
            if (found < cap)
                out[found] = (uint32_t)i;
            found++;
        }
    }
    return found;
}

size_t tmb_pick_knearest(tmb_world *w, tmb_vec2 p, size_t k, uint32_t *out)
{
    size_t n = tmb_world_body_count(w);
    if (k == 0 || n == 0)
        return 0;
    size_t written = 0;
    /* simple selection: maintain a sorted prefix of length `written` */
    float best_d[64];
    if (k > 64)
        k = 64;
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        float dx = b->pos.x - p.x;
        float dy = b->pos.y - p.y;
        float d = dx * dx + dy * dy;
        if (written < k) {
            size_t j = written++;
            while (j > 0 && best_d[j - 1] > d) {
                best_d[j] = best_d[j - 1];
                out[j] = out[j - 1];
                j--;
            }
            best_d[j] = d;
            out[j] = (uint32_t)i;
        } else if (d < best_d[written - 1]) {
            size_t j = written - 1;
            while (j > 0 && best_d[j - 1] > d) {
                best_d[j] = best_d[j - 1];
                out[j] = out[j - 1];
                j--;
            }
            best_d[j] = d;
            out[j] = (uint32_t)i;
        }
    }
    return written;
}

void tmb_pick_census(tmb_world *w, tmb_aabb region, size_t *dyn, size_t *stat)
{
    size_t d = 0, s = 0;
    size_t n = tmb_world_body_count(w);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        if (!b)
            continue;
        if (!tmb_aabb_contains(region, b->pos))
            continue;
        if (b->type == TMB_BODY_DYNAMIC)
            d++;
        else
            s++;
    }
    if (dyn)
        *dyn = d;
    if (stat)
        *stat = s;
}
