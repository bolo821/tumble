#ifndef TUMBLE_RNG_H
#define TUMBLE_RNG_H
#include "tumble/vec2.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* A small deterministic PRNG (SplitMix64-seeded xorshift) so scenes and tests
 * reproduce exactly. */
typedef struct { uint64_t state; } tmb_rng;
void tmb_rng_seed(tmb_rng *r, uint64_t seed);
uint32_t tmb_rng_u32(tmb_rng *r);
float tmb_rng_float(tmb_rng *r);              /* [0,1) */
float tmb_rng_range(tmb_rng *r, float lo, float hi);
tmb_vec2 tmb_rng_in_disk(tmb_rng *r, float radius);
#ifdef __cplusplus
}
#endif
#endif
