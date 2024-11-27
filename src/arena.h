#ifndef TUMBLE_ARENA_H
#define TUMBLE_ARENA_H
#include <stddef.h>
typedef struct tmb_arena_block tmb_arena_block;
typedef struct { tmb_arena_block *head; size_t block_size; size_t total; } tmb_arena;
void tmb_arena_init(tmb_arena *a);
void tmb_arena_free(tmb_arena *a);
void *tmb_arena_alloc(tmb_arena *a, size_t n);
char *tmb_arena_strndup(tmb_arena *a, const char *s, size_t n);
#endif
