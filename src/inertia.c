#include "tumble/inertia.h"

tmb_mass_data tmb_mass_of_shape(const tmb_shape *s, float density,
                                tmb_vec2 offset)
{
    tmb_mass_data md;
    tmb_vec2 c;
    tmb_shape_compute_mass(s, density, &md.mass, &md.inertia, &c);
    md.center = tmb_v2_add(c, offset);
    /* inertia returned is about the shape centroid; shift to the offset frame
     * origin is not needed yet -- callers combine via the accumulator */
    return md;
}

void tmb_mass_begin(tmb_mass_accum *acc)
{
    acc->total.mass = 0;
    acc->total.center = tmb_v2(0, 0);
    acc->total.inertia = 0;
    acc->count = 0;
}

void tmb_mass_add(tmb_mass_accum *acc, const tmb_shape *s, float density,
                  tmb_vec2 offset)
{
    float mass, inertia;
    tmb_vec2 centroid;
    tmb_shape_compute_mass(s, density, &mass, &inertia, &centroid);
    tmb_vec2 world_c = tmb_v2_add(centroid, offset);
    /* accumulate mass and mass-weighted center */
    acc->total.center = tmb_v2_add(
        tmb_v2_scale(acc->total.center, acc->total.mass),
        tmb_v2_scale(world_c, mass));
    acc->total.mass += mass;
    if (acc->total.mass > 0)
        acc->total.center = tmb_v2_scale(acc->total.center, 1.0f / acc->total.mass);
    /* inertia about this piece's own center, then shift to global origin via
     * the parallel-axis theorem; we recenter to the COM in finish() */
    float d2 = tmb_v2_dot(world_c, world_c);
    acc->total.inertia += inertia + mass * d2;
    acc->count++;
}

tmb_mass_data tmb_mass_finish(tmb_mass_accum *acc)
{
    tmb_mass_data md = acc->total;
    if (md.mass > 0) {
        /* shift inertia from the origin to the center of mass */
        float d2 = tmb_v2_dot(md.center, md.center);
        md.inertia -= md.mass * d2;
        if (md.inertia < 0)
            md.inertia = 0;
    }
    return md;
}
