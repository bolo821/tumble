#ifndef TUMBLE_FILTER_H
#define TUMBLE_FILTER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Collision filtering by category/mask bits plus a group override (Box2D
 * style): same positive group always collides, same negative group never. */
typedef struct { uint32_t category; uint32_t mask; int32_t group; } tmb_filter;
tmb_filter tmb_filter_default(void);
int tmb_filter_should_collide(tmb_filter a, tmb_filter b);
#ifdef __cplusplus
}
#endif
#endif
