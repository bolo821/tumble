#include "tumble/easing.h"
#include <math.h>
static float clamp01(float t){ return t<0?0:(t>1?1:t); }
float tmb_ease_linear(float t){ return clamp01(t); }
float tmb_ease_in_quad(float t){ t=clamp01(t); return t*t; }
float tmb_ease_out_quad(float t){ t=clamp01(t); return 1.0f-(1.0f-t)*(1.0f-t); }
float tmb_ease_inout_quad(float t){ t=clamp01(t); return t<0.5f?2*t*t:1-(-2*t+2)*(-2*t+2)/2; }
float tmb_ease_in_cubic(float t){ t=clamp01(t); return t*t*t; }
float tmb_ease_out_cubic(float t){ t=clamp01(t); float u=1-t; return 1-u*u*u; }
float tmb_ease_out_bounce(float t){
    t=clamp01(t); const float n=7.5625f,d=2.75f;
    if(t<1/d) return n*t*t;
    if(t<2/d){ t-=1.5f/d; return n*t*t+0.75f; }
    if(t<2.5f/d){ t-=2.25f/d; return n*t*t+0.9375f; }
    t-=2.625f/d; return n*t*t+0.984375f;
}
float tmb_ease_out_elastic(float t){
    t=clamp01(t); if(t==0||t==1) return t;
    float c=(2*3.14159265f)/3.0f;
    return powf(2,-10*t)*sinf((t*10-0.75f)*c)+1;
}
float tmb_smoothstep(float a, float b, float x){
    if(a==b) return x<a?0:1;
    float t=clamp01((x-a)/(b-a)); return t*t*(3-2*t);
}
