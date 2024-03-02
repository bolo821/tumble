#include "tumble/bitset.h"

#include <stdlib.h>
#include <string.h>

static size_t words_for(size_t nbits) { return (nbits + 63) / 64; }

int tmb_bitset_init(tmb_bitset *b, size_t nbits)
{
    b->nwords = words_for(nbits);
    b->nbits = nbits;
    b->words = b->nwords ? (uint64_t *)calloc(b->nwords, sizeof(uint64_t))
                         : NULL;
    return (b->nwords && !b->words) ? -1 : 0;
}

void tmb_bitset_free(tmb_bitset *b)
{
    free(b->words);
    b->words = NULL;
    b->nbits = b->nwords = 0;
}

int tmb_bitset_resize(tmb_bitset *b, size_t nbits)
{
    size_t nw = words_for(nbits);
    if (nw != b->nwords) {
        uint64_t *w = (uint64_t *)realloc(b->words, nw * sizeof(uint64_t));
        if (nw && !w)
            return -1;
        if (nw > b->nwords && w)
            memset(w + b->nwords, 0, (nw - b->nwords) * sizeof(uint64_t));
        b->words = w;
        b->nwords = nw;
    }
    b->nbits = nbits;
    return 0;
}

void tmb_bitset_set(tmb_bitset *b, size_t i)
{
    if (i < b->nbits)
        b->words[i >> 6] |= (uint64_t)1 << (i & 63);
}

void tmb_bitset_clear(tmb_bitset *b, size_t i)
{
    if (i < b->nbits)
        b->words[i >> 6] &= ~((uint64_t)1 << (i & 63));
}

int tmb_bitset_test(const tmb_bitset *b, size_t i)
{
    if (i >= b->nbits)
        return 0;
    return (b->words[i >> 6] >> (i & 63)) & 1u;
}

void tmb_bitset_clear_all(tmb_bitset *b)
{
    if (b->words)
        memset(b->words, 0, b->nwords * sizeof(uint64_t));
}

size_t tmb_bitset_count(const tmb_bitset *b)
{
    size_t total = 0;
    for (size_t w = 0; w < b->nwords; w++) {
        uint64_t x = b->words[w];
        while (x) {
            x &= x - 1; /* clear lowest set bit */
            total++;
        }
    }
    return total;
}

size_t tmb_bitset_next(const tmb_bitset *b, size_t from)
{
    for (size_t i = from; i < b->nbits; i++) {
        if ((b->words[i >> 6] >> (i & 63)) & 1u)
            return i;
    }
    return (size_t)-1;
}
