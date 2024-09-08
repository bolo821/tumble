#ifndef TUMBLE_EASING_H
#define TUMBLE_EASING_H
#ifdef __cplusplus
extern "C" {
#endif
/* Scalar easing curves on t in [0,1], for animating spawners and cameras. */
float tmb_ease_linear(float t);
float tmb_ease_in_quad(float t);
float tmb_ease_out_quad(float t);
float tmb_ease_inout_quad(float t);
float tmb_ease_in_cubic(float t);
float tmb_ease_out_cubic(float t);
float tmb_ease_out_bounce(float t);
float tmb_ease_out_elastic(float t);
float tmb_smoothstep(float a, float b, float x);
#ifdef __cplusplus
}
#endif
#endif
