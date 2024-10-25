#include "tumble/stats.h"
#include "tumble/motion.h"

#include <string.h>

void tmb_world_stats(tmb_world *w, tmb_stats *out)
{
    memset(out, 0, sizeof(*out));
    out->bodies = w->body_count;
    out->joints = w->joint_count;
    for (size_t i = 0; i < w->body_count; i++) {
        tmb_body *b = &w->bodies[i];
        if (b->type == TMB_BODY_STATIC) out->static_bodies++;
        else out->dynamic_bodies++;
        if (b->shape.kind == TMB_SHAPE_CIRCLE) out->circles++;
        else out->polygons++;
    }
    out->total_mass = tmb_world_total_mass(w);
    out->kinetic_energy = tmb_world_kinetic_energy(w);
    out->bounds = tmb_world_bounds(w);
}

void tmb_stats_print(const tmb_stats *s, tmb_buf *out)
{
    tmb_buf_printf(out, "bodies:        %zu (%zu static, %zu dynamic)\n",
                   s->bodies, s->static_bodies, s->dynamic_bodies);
    tmb_buf_printf(out, "shapes:        %zu circles, %zu polygons\n",
                   s->circles, s->polygons);
    tmb_buf_printf(out, "joints:        %zu\n", s->joints);
    tmb_buf_printf(out, "total mass:    %.6g\n", s->total_mass);
    tmb_buf_printf(out, "kinetic energy:%.6g\n", s->kinetic_energy);
    if (s->bounds.min.x <= s->bounds.max.x)
        tmb_buf_printf(out, "bounds:        (%.4g,%.4g)-(%.4g,%.4g)\n",
                       s->bounds.min.x, s->bounds.min.y, s->bounds.max.x,
                       s->bounds.max.y);
}
