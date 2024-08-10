#include "tumble/noise.h"

#include <math.h>

void tmb_noise_init(tmb_noise *n, uint32_t seed)
{
    for (int i = 0; i < 256; i++)
        n->perm[i] = (uint8_t)i;
    /* shuffle with a deterministic LCG */
    uint32_t s = seed ? seed : 1u;
    for (int i = 255; i > 0; i--) {
        s = s * 1664525u + 1013904223u;
        int j = (int)(s % (uint32_t)(i + 1));
        uint8_t t = n->perm[i];
        n->perm[i] = n->perm[j];
        n->perm[j] = t;
    }
    for (int i = 0; i < 256; i++)
        n->perm[256 + i] = n->perm[i];
}

static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static float lerp(float a, float b, float t) { return a + t * (b - a); }

static float grad(int hash, float x, float y)
{
    switch (hash & 7) {
    case 0: return x + y;
    case 1: return -x + y;
    case 2: return x - y;
    case 3: return -x - y;
    case 4: return x;
    case 5: return -x;
    case 6: return y;
    default: return -y;
    }
}

float tmb_perlin(const tmb_noise *n, float x, float y)
{
    int xi = (int)floorf(x) & 255;
    int yi = (int)floorf(y) & 255;
    float xf = x - floorf(x);
    float yf = y - floorf(y);
    float u = fade(xf), v = fade(yf);
    int aa = n->perm[n->perm[xi] + yi];
    int ab = n->perm[n->perm[xi] + yi + 1];
    int ba = n->perm[n->perm[xi + 1] + yi];
    int bb = n->perm[n->perm[xi + 1] + yi + 1];
    float x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
    return lerp(x1, x2, v);
}

float tmb_value_noise(const tmb_noise *n, float x, float y)
{
    int xi = (int)floorf(x) & 255;
    int yi = (int)floorf(y) & 255;
    float xf = fade(x - floorf(x));
    float yf = fade(y - floorf(y));
    float v00 = (float)n->perm[n->perm[xi] + yi] / 255.0f;
    float v10 = (float)n->perm[n->perm[xi + 1] + yi] / 255.0f;
    float v01 = (float)n->perm[n->perm[xi] + yi + 1] / 255.0f;
    float v11 = (float)n->perm[n->perm[xi + 1] + yi + 1] / 255.0f;
    return lerp(lerp(v00, v10, xf), lerp(v01, v11, xf), yf) * 2.0f - 1.0f;
}

float tmb_fbm(const tmb_noise *n, float x, float y, int octaves, float lac,
              float gain)
{
    float sum = 0, amp = 0.5f, freq = 1.0f, norm = 0;
    if (octaves < 1)
        octaves = 1;
    for (int i = 0; i < octaves; i++) {
        sum += amp * tmb_perlin(n, x * freq, y * freq);
        norm += amp;
        amp *= gain;
        freq *= lac;
    }
    return norm > 0 ? sum / norm : 0;
}

tmb_vec2 tmb_noise_flow(const tmb_noise *n, float x, float y)
{
    /* curl of a scalar potential gives a swirling, low-divergence field */
    const float e = 0.01f;
    float dphidx = (tmb_perlin(n, x + e, y) - tmb_perlin(n, x - e, y)) / (2 * e);
    float dphidy = (tmb_perlin(n, x, y + e) - tmb_perlin(n, x, y - e)) / (2 * e);
    return tmb_v2(dphidy, -dphidx);
}
