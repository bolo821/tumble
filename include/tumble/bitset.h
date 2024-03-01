#ifndef TUMBLE_BITSET_H
#define TUMBLE_BITSET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A dynamic bit array. The solver and broadphase use it to mark visited
 * bodies, awake islands and processed pairs without a per-element allocation. */

typedef struct {
    uint64_t *words;
    size_t nbits;
    size_t nwords;
} tmb_bitset;

int tmb_bitset_init(tmb_bitset *b, size_t nbits);
void tmb_bitset_free(tmb_bitset *b);
int tmb_bitset_resize(tmb_bitset *b, size_t nbits);
void tmb_bitset_set(tmb_bitset *b, size_t i);
void tmb_bitset_clear(tmb_bitset *b, size_t i);
int tmb_bitset_test(const tmb_bitset *b, size_t i);
void tmb_bitset_clear_all(tmb_bitset *b);
size_t tmb_bitset_count(const tmb_bitset *b);
/* Index of the next set bit at or after `from`, or (size_t)-1. */
size_t tmb_bitset_next(const tmb_bitset *b, size_t from);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BITSET_H */
