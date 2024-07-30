#ifndef TUMBLE_WIND_H
#define TUMBLE_WIND_H

#include "tumble/world.h"
#include "tumble/noise.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A spatially-varying wind field driven by curl noise, applied as a drag-style
 * force that pushes bodies toward the local flow velocity. Gives leaves, cloth
 * and debris organic motion. */

typedef struct {
    tmb_noise noise;
    float scale;     /* spatial frequency of the field */
    float strength;  /* peak flow speed */
    float drag;      /* how strongly bodies follow the flow */
    float time;      /* advanced each apply to animate the field */
} tmb_wind;

void tmb_wind_init(tmb_wind *wind, uint32_t seed, float strength);
tmb_vec2 tmb_wind_sample(const tmb_wind *wind, tmb_vec2 p);
/* Apply the field to all dynamic bodies and advance the animation by dt. */
size_t tmb_wind_apply(tmb_wind *wind, tmb_world *w, float dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_WIND_H */
