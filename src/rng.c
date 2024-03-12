#include "tumble/rng.h"
#include <math.h>
void tmb_rng_seed(tmb_rng *r, uint64_t seed){ r->state = seed ? seed : 0x9e3779b97f4a7c15ull; }
uint32_t tmb_rng_u32(tmb_rng *r){
    uint64_t x=r->state; x^=x<<13; x^=x>>7; x^=x<<17; r->state=x;
    return (uint32_t)(x>>32);
}
float tmb_rng_float(tmb_rng *r){ return (float)(tmb_rng_u32(r)>>8)/(float)(1u<<24); }
float tmb_rng_range(tmb_rng *r, float lo, float hi){ return lo + (hi-lo)*tmb_rng_float(r); }
tmb_vec2 tmb_rng_in_disk(tmb_rng *r, float radius){
    float a=tmb_rng_float(r)*6.2831853f; float rr=radius*sqrtf(tmb_rng_float(r));
    return tmb_v2(cosf(a)*rr, sinf(a)*rr);
}
