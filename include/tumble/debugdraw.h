#ifndef TUMBLE_DEBUGDRAW_H
#define TUMBLE_DEBUGDRAW_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A backend-agnostic debug renderer. The world is decomposed into line
 * segments, circles and points tagged with a color; a frontend (GL, SVG,
 * canvas, a test) consumes them through the callbacks. Nothing here touches a
 * graphics API, so it is trivially testable. */

typedef struct {
    float r, g, b, a;
} tmb_color;

typedef struct tmb_ddraw tmb_ddraw;
struct tmb_ddraw {
    void *user;
    void (*segment)(tmb_ddraw *d, tmb_vec2 a, tmb_vec2 b, tmb_color c);
    void (*circle)(tmb_ddraw *d, tmb_vec2 center, float radius, tmb_color c);
    void (*point)(tmb_ddraw *d, tmb_vec2 p, float size, tmb_color c);
    /* which layers to emit */
    int draw_shapes;
    int draw_aabbs;
    int draw_contacts;
    int draw_joints;
    int draw_centers;
    /* running tallies, useful for tests */
    size_t segments_emitted;
    size_t circles_emitted;
    size_t points_emitted;
};

void tmb_ddraw_init(tmb_ddraw *d);
tmb_color tmb_rgb(float r, float g, float b);

/* Emit the whole world according to the enabled layers. */
void tmb_ddraw_world(tmb_ddraw *d, tmb_world *w);
/* Emit a single body's shape outline. */
void tmb_ddraw_body(tmb_ddraw *d, const tmb_body *b, tmb_color c);
void tmb_ddraw_aabb(tmb_ddraw *d, tmb_aabb box, tmb_color c);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_DEBUGDRAW_H */
