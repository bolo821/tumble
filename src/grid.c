#include "tumble/grid.h"
#include "tumble/shape.h"
#include "tumble/transform.h"

#include <stdlib.h>
#include <math.h>

typedef struct {
    uint64_t key;
    uint32_t *items;
    size_t count, cap;
} cell;

struct tmb_grid {
    tmb_aabb *boxes; /* per body */
    size_t nbody;
    float cell;
    tmb_vec2 origin;
    cell *cells;
    size_t cell_count, cell_cap;
    uint32_t *index; /* open addressing -> cell index + 1 */
    size_t index_cap;
};

static uint64_t mix(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdull;
    k ^= k >> 33;
    return k;
}
static uint64_t ckey(long x, long y)
{
    return ((uint64_t)(uint32_t)x) | ((uint64_t)(uint32_t)y << 32);
}

static int idx_grow(tmb_grid *g)
{
    size_t nc = g->index_cap ? g->index_cap * 2 : 1024;
    uint32_t *ni = (uint32_t *)calloc(nc, sizeof(uint32_t));
    if (!ni)
        return -1;
    for (size_t i = 0; i < g->index_cap; i++) {
        uint32_t s = g->index[i];
        if (!s)
            continue;
        size_t j = (size_t)mix(g->cells[s - 1].key) & (nc - 1);
        while (ni[j])
            j = (j + 1) & (nc - 1);
        ni[j] = s;
    }
    free(g->index);
    g->index = ni;
    g->index_cap = nc;
    return 0;
}

static cell *get_cell(tmb_grid *g, uint64_t key, int create)
{
    if (g->index_cap == 0) {
        if (!create)
            return NULL;
        if (idx_grow(g) != 0)
            return NULL;
    }
    size_t mask = g->index_cap - 1;
    size_t i = (size_t)mix(key) & mask;
    while (g->index[i]) {
        if (g->cells[g->index[i] - 1].key == key)
            return &g->cells[g->index[i] - 1];
        i = (i + 1) & mask;
    }
    if (!create)
        return NULL;
    if ((g->cell_count + 1) * 4 >= g->index_cap * 3) {
        if (idx_grow(g) != 0)
            return NULL;
        return get_cell(g, key, create);
    }
    if (g->cell_count == g->cell_cap) {
        size_t nc = g->cell_cap ? g->cell_cap * 2 : 256;
        cell *ncl = (cell *)realloc(g->cells, nc * sizeof(*ncl));
        if (!ncl)
            return NULL;
        g->cells = ncl;
        g->cell_cap = nc;
    }
    cell *c = &g->cells[g->cell_count];
    c->key = key;
    c->items = NULL;
    c->count = c->cap = 0;
    g->index[i] = (uint32_t)g->cell_count + 1;
    g->cell_count++;
    return c;
}

static int cell_push(cell *c, uint32_t v)
{
    if (c->count == c->cap) {
        size_t nc = c->cap ? c->cap * 2 : 4;
        uint32_t *ni = (uint32_t *)realloc(c->items, nc * sizeof(*ni));
        if (!ni)
            return -1;
        c->items = ni;
        c->cap = nc;
    }
    c->items[c->count++] = v;
    return 0;
}

tmb_grid *tmb_grid_build(tmb_world *w, float cell_size)
{
    if (cell_size < 1e-4f)
        cell_size = 1e-4f;
    tmb_grid *g = (tmb_grid *)calloc(1, sizeof(*g));
    if (!g)
        return NULL;
    g->nbody = tmb_world_body_count(w);
    g->cell = cell_size;
    g->origin = tmb_v2(0, 0);
    g->boxes = (tmb_aabb *)malloc((g->nbody ? g->nbody : 1) * sizeof(tmb_aabb));
    if (!g->boxes) {
        free(g);
        return NULL;
    }
    for (size_t i = 0; i < g->nbody; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        g->boxes[i] = tmb_shape_aabb(&b->shape, b->xf);
        tmb_aabb bx = g->boxes[i];
        long x0 = (long)floorf(bx.min.x / cell_size);
        long y0 = (long)floorf(bx.min.y / cell_size);
        long x1 = (long)floorf(bx.max.x / cell_size);
        long y1 = (long)floorf(bx.max.y / cell_size);
        /* cap the cells a single body may touch, so a huge AABB cannot blow up
         * the grid */
        if ((x1 - x0 + 1) * (y1 - y0 + 1) > 4096)
            continue;
        for (long y = y0; y <= y1; y++)
            for (long x = x0; x <= x1; x++) {
                cell *c = get_cell(g, ckey(x, y), 1);
                if (!c || cell_push(c, (uint32_t)i) != 0) {
                    tmb_grid_free(g);
                    return NULL;
                }
            }
    }
    return g;
}

void tmb_grid_free(tmb_grid *g)
{
    if (!g)
        return;
    for (size_t i = 0; i < g->cell_count; i++)
        free(g->cells[i].items);
    free(g->cells);
    free(g->index);
    free(g->boxes);
    free(g);
}

void tmb_grid_occupancy(const tmb_grid *g, int *counts, int side)
{
    if (side <= 0)
        return;
    for (int i = 0; i < side * side; i++)
        counts[i] = 0;
    /* Fold each body's cell coordinates into a side x side heatmap so callers
     * can see how the scene is distributed across the grid.  Cell coordinates
     * are signed and are frequently negative (any body below/left of the
     * origin), so the fold must use a floored modulo that always lands in
     * [0, side) -- a plain '%' keeps the sign of the dividend and would index
     * before the start of counts. */
    for (size_t i = 0; i < g->nbody; i++) {
        long cx = (long)floorf(g->boxes[i].min.x / g->cell);
        long cy = (long)floorf(g->boxes[i].min.y / g->cell);
        int bx = tmb_floor_mod(cx, side);
        int by = tmb_floor_mod(cy, side);
        counts[by * side + bx]++;
    }
}

static size_t gather(const tmb_grid *g, long x0, long y0, long x1, long y1,
                     tmb_aabb box, uint32_t *out, size_t cap)
{
    size_t found = 0;
    for (long y = y0; y <= y1; y++) {
        for (long x = x0; x <= x1; x++) {
            cell *c = get_cell((tmb_grid *)g, ckey(x, y), 0);
            if (!c)
                continue;
            for (size_t i = 0; i < c->count; i++) {
                uint32_t b = c->items[i];
                if (!tmb_aabb_overlap(box, g->boxes[b]))
                    continue;
                /* dedup: only report from the cell containing the box min */
                long bx0 = (long)floorf(g->boxes[b].min.x / g->cell);
                long by0 = (long)floorf(g->boxes[b].min.y / g->cell);
                if (bx0 != x || by0 != y) {
                    if (bx0 >= x0 && by0 >= y0)
                        continue; /* will be reported from its min cell */
                }
                if (found < cap)
                    out[found] = b;
                found++;
            }
        }
    }
    return found;
}

size_t tmb_grid_query_aabb(const tmb_grid *g, tmb_aabb box, uint32_t *out,
                           size_t cap)
{
    long x0 = (long)floorf(box.min.x / g->cell);
    long y0 = (long)floorf(box.min.y / g->cell);
    long x1 = (long)floorf(box.max.x / g->cell);
    long y1 = (long)floorf(box.max.y / g->cell);
    if ((x1 - x0 + 1) * (y1 - y0 + 1) > 65536)
        return 0;
    return gather(g, x0, y0, x1, y1, box, out, cap);
}

size_t tmb_grid_query_point(const tmb_grid *g, tmb_vec2 p, uint32_t *out,
                            size_t cap)
{
    tmb_aabb box = {p, p};
    long x = (long)floorf(p.x / g->cell);
    long y = (long)floorf(p.y / g->cell);
    return gather(g, x, y, x, y, box, out, cap);
}
