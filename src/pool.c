#include "tumble/pool.h"

#include <stdlib.h>
#include <string.h>

struct tmb_pool_chunk {
    tmb_pool_chunk *next;
    unsigned char *mem;
};

int tmb_pool_init(tmb_pool *p, size_t block_size, size_t blocks_per_chunk)
{
    if (block_size < sizeof(void *))
        block_size = sizeof(void *); /* must hold a free-list pointer */
    if (blocks_per_chunk == 0)
        blocks_per_chunk = 64;
    p->block_size = block_size;
    p->blocks_per_chunk = blocks_per_chunk;
    p->free_list = NULL;
    p->chunks = NULL;
    p->live = 0;
    p->total = 0;
    return 0;
}

void tmb_pool_free(tmb_pool *p)
{
    tmb_pool_chunk *c = p->chunks;
    while (c) {
        tmb_pool_chunk *next = c->next;
        free(c->mem);
        free(c);
        c = next;
    }
    p->chunks = NULL;
    p->free_list = NULL;
    p->live = p->total = 0;
}

static int grow(tmb_pool *p)
{
    tmb_pool_chunk *c = (tmb_pool_chunk *)malloc(sizeof(*c));
    if (!c)
        return -1;
    c->mem = (unsigned char *)malloc(p->block_size * p->blocks_per_chunk);
    if (!c->mem) {
        free(c);
        return -1;
    }
    c->next = p->chunks;
    p->chunks = c;
    /* thread every block in the new chunk onto the free list */
    for (size_t i = 0; i < p->blocks_per_chunk; i++) {
        void *block = c->mem + i * p->block_size;
        *(void **)block = p->free_list;
        p->free_list = block;
    }
    p->total += p->blocks_per_chunk;
    return 0;
}

void *tmb_pool_alloc(tmb_pool *p)
{
    if (!p->free_list && grow(p) != 0)
        return NULL;
    void *block = p->free_list;
    p->free_list = *(void **)block;
    p->live++;
    return block;
}

void *tmb_pool_calloc(tmb_pool *p)
{
    void *block = tmb_pool_alloc(p);
    if (block)
        memset(block, 0, p->block_size);
    return block;
}

void tmb_pool_release(tmb_pool *p, void *block)
{
    if (!block)
        return;
    *(void **)block = p->free_list;
    p->free_list = block;
    if (p->live > 0)
        p->live--;
}

void tmb_pool_reset(tmb_pool *p)
{
    p->free_list = NULL;
    for (tmb_pool_chunk *c = p->chunks; c; c = c->next) {
        for (size_t i = 0; i < p->blocks_per_chunk; i++) {
            void *block = c->mem + i * p->block_size;
            *(void **)block = p->free_list;
            p->free_list = block;
        }
    }
    p->live = 0;
}
