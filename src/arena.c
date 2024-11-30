#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct tmb_arena_block { tmb_arena_block *next; size_t used, cap; unsigned char data[]; };
#define TMB_ARENA_DEFAULT (16 * 1024)
#define TMB_ARENA_ALIGN 16

void tmb_arena_init(tmb_arena *a) { a->head = NULL; a->block_size = TMB_ARENA_DEFAULT; a->total = 0; }

void tmb_arena_free(tmb_arena *a)
{
    tmb_arena_block *b = a->head;
    while (b) { tmb_arena_block *n = b->next; free(b); b = n; }
    a->head = NULL; a->total = 0;
}

void *tmb_arena_alloc(tmb_arena *a, size_t n)
{
    if (n == 0) n = 1;
    size_t want = (n + (TMB_ARENA_ALIGN - 1)) & ~(size_t)(TMB_ARENA_ALIGN - 1);
    if (want < n) return NULL;
    tmb_arena_block *b = a->head;
    if (!b || b->used + want > b->cap) {
        size_t cap = a->block_size; if (cap < want) cap = want;
        if (cap > SIZE_MAX - sizeof(tmb_arena_block)) return NULL;
        b = (tmb_arena_block *)malloc(sizeof(tmb_arena_block) + cap);
        if (!b) return NULL;
        b->next = a->head; b->used = 0; b->cap = cap; a->head = b;
    }
    void *p = b->data + b->used; b->used += want; a->total += want;
    return p;
}

char *tmb_arena_strndup(tmb_arena *a, const char *s, size_t n)
{
    char *p = (char *)tmb_arena_alloc(a, n + 1);
    if (p) { memcpy(p, s, n); p[n] = '\0'; }
    return p;
}
