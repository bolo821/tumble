#ifndef TUMBLE_POOL_H
#define TUMBLE_POOL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A fixed-size block allocator. Blocks are carved from larger chunks and freed
 * blocks are kept on an embedded free list, so allocation and release are O(1)
 * with no per-block malloc overhead. Used for the many small, same-sized
 * records (contacts, tree nodes) a simulation churns through. */

typedef struct tmb_pool_chunk tmb_pool_chunk;

typedef struct {
    size_t block_size;
    size_t blocks_per_chunk;
    void *free_list;
    tmb_pool_chunk *chunks;
    size_t live;       /* outstanding allocations */
    size_t total;      /* blocks ever carved */
} tmb_pool;

int tmb_pool_init(tmb_pool *p, size_t block_size, size_t blocks_per_chunk);
void tmb_pool_free(tmb_pool *p);
void *tmb_pool_alloc(tmb_pool *p);
void tmb_pool_release(tmb_pool *p, void *block);
/* Release every block at once without freeing the backing chunks. */
void tmb_pool_reset(tmb_pool *p);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_POOL_H */
