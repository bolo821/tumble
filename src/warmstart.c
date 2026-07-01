#include "tumble/warmstart.h"

#include <stdlib.h>
#include <string.h>

void tmb_ws_cache_init(tmb_ws_cache *c)
{
    memset(c, 0, sizeof(*c));
}

void tmb_ws_cache_free(tmb_ws_cache *c)
{
    free(c->table);
    free(c->slab);
    tmb_ws_cache_init(c);
}

static uint64_t ws_pack(uint32_t a, uint32_t b)
{
    uint32_t lo = a < b ? a : b;
    uint32_t hi = a < b ? b : a;
    return ((uint64_t)lo << 32) | (uint64_t)hi;
}

static int ws_table_grow(tmb_ws_cache *c)
{
    size_t newcap = c->table_cap ? c->table_cap * 2 : 16;
    tmb_ws_slot *nt = (tmb_ws_slot *)calloc(newcap, sizeof(*nt));
    if (!nt)
        return 0;
    /* Rehash */
    for (size_t i = 0; i < c->table_cap; i++) {
        if (!c->table[i].pair)
            continue;
        uint64_t p = c->table[i].pair;
        size_t h = (size_t)(p >> 32) ^ (size_t)(p & 0xffffffffu);
        for (size_t step = 0; step < newcap; step++) {
            size_t idx = (h + step) & (newcap - 1);
            if (!nt[idx].pair) {
                nt[idx] = c->table[i];
                break;
            }
        }
    }
    free(c->table);
    c->table = nt;
    c->table_cap = newcap;
    return 1;
}

static tmb_ws_slot *ws_slot(tmb_ws_cache *c, uint64_t pair)
{
    if (c->table_count * 2 >= c->table_cap)
        if (!ws_table_grow(c))
            return NULL;
    size_t h = (size_t)(pair >> 32) ^ (size_t)(pair & 0xffffffffu);
    for (size_t step = 0; step < c->table_cap; step++) {
        size_t idx = (h + step) & (c->table_cap - 1);
        if (!c->table[idx].pair || c->table[idx].pair == pair) {
            if (!c->table[idx].pair)
                c->table_count++;
            c->table[idx].pair = pair;
            return &c->table[idx];
        }
    }
    return NULL;
}

static tmb_ws_slot *ws_find(const tmb_ws_cache *c, uint64_t pair)
{
    if (!c->table_cap)
        return NULL;
    size_t h = (size_t)(pair >> 32) ^ (size_t)(pair & 0xffffffffu);
    for (size_t step = 0; step < c->table_cap; step++) {
        size_t idx = (h + step) & (c->table_cap - 1);
        if (!c->table[idx].pair)
            return NULL;
        if (c->table[idx].pair == pair)
            return &c->table[idx];
    }
    return NULL;
}

void tmb_ws_store(tmb_ws_cache *c, const tmb_world *w)
{
    /* Reset the slab index so we overwrite stale copies from last step. */
    c->slab_count = 0;

    /* Clear the hash table occupancy so stale pairs are not carried forward. */
    if (c->table_cap)
        memset(c->table, 0, c->table_cap * sizeof(*c->table));
    c->table_count = 0;

    for (size_t i = 0; i < w->contact_count; i++) {
        const tmb_contact *cur = &w->contacts[i];
        uint64_t pair = ws_pack(cur->a, cur->b);

        tmb_ws_slot *slot = ws_slot(c, pair);
        if (!slot)
            continue;

        if (c->slab_count == c->slab_cap) {
            size_t nc = c->slab_cap ? c->slab_cap * 2 : 4;
            tmb_contact *old_base = c->slab;
            tmb_contact *ns =
                (tmb_contact *)realloc(c->slab, nc * sizeof(*ns));
            if (!ns) {
                slot->prev = NULL;
                continue;
            }
            /* If realloc moved the slab, patch every stored prev pointer so
             * tmb_ws_apply does not read through a dangling address. */
            if (ns != old_base && old_base) {
                for (size_t k = 0; k < c->table_cap; k++) {
                    if (!c->table[k].pair || !c->table[k].prev)
                        continue;
                    ptrdiff_t off = c->table[k].prev - old_base;
                    if (off >= 0 && (size_t)off < c->slab_count)
                        c->table[k].prev = ns + off;
                }
            }
            c->slab = ns;
            c->slab_cap = nc;
        }

        c->slab[c->slab_count++] = *cur;
        slot->prev = &c->slab[c->slab_count - 1];
    }
}

void tmb_ws_apply(const tmb_ws_cache *c, tmb_world *w)
{
    if (!c->table_cap)
        return;
    for (size_t i = 0; i < w->contact_count; i++) {
        tmb_contact *cur = &w->contacts[i];
        uint64_t pair = ws_pack(cur->a, cur->b);
        const tmb_ws_slot *slot = ws_find(c, pair);
        if (slot && slot->prev) {
            cur->normal_impulse  = slot->prev->normal_impulse;
            cur->tangent_impulse = slot->prev->tangent_impulse;
        }
    }
}
