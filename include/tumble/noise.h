#ifndef TUMBLE_NOISE_H
#define TUMBLE_NOISE_H

#include "tumble/vec2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Value and gradient (Perlin-style) noise, plus fractal Brownian motion. Used
 * to drive wind fields, procedural terrain heightlines and scatter patterns in
 * the scene generators. Deterministic for a given seed. */

typedef struct {
    uint8_t perm[512];
} tmb_noise;

void tmb_noise_init(tmb_noise *n, uint32_t seed);
float tmb_value_noise(const tmb_noise *n, float x, float y);
float tmb_perlin(const tmb_noise *n, float x, float y);
/* Sum `octaves` of Perlin noise with the given lacunarity/gain. */
float tmb_fbm(const tmb_noise *n, float x, float y, int octaves, float lac,
              float gain);
/* A divergence-free-ish 2D flow direction sampled from the field. */
tmb_vec2 tmb_noise_flow(const tmb_noise *n, float x, float y);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_NOISE_H */
