#ifndef TUMBLE_STATS_H
#define TUMBLE_STATS_H

#include "tumble/world.h"
#include "tumble/aabb.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t bodies;
    size_t static_bodies;
    size_t dynamic_bodies;
    size_t circles;
    size_t polygons;
    size_t joints;
    float total_mass;
    float kinetic_energy;
    tmb_aabb bounds;
} tmb_stats;

void tmb_world_stats(tmb_world *w, tmb_stats *out);
void tmb_stats_print(const tmb_stats *s, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_STATS_H */
