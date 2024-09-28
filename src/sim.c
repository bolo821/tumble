#include "tumble/sim.h"

tmb_sim_opts tmb_sim_defaults(void)
{
    tmb_sim_opts o;
    o.enable_sleeping = 1;
    o.enable_recording = 0;
    o.enable_profiling = 1;
    o.fixed_dt = 1.0f / 60.0f;
    o.record_bodies = 0;
    return o;
}

void tmb_sim_init(tmb_sim *s, tmb_world *w, tmb_sim_opts opts)
{
    s->world = w;
    s->opts = opts;
    s->steps = 0;
    s->sleeping = 0;
    tmb_islands_init(&s->islands);
    tmb_timestep_init(&s->ts, opts.fixed_dt);
    tmb_profile_init(&s->profile);
    size_t rb = opts.record_bodies ? opts.record_bodies
                                   : tmb_world_body_count(w);
    if (rb == 0)
        rb = 1;
    tmb_recorder_init(&s->recorder, rb);
}

void tmb_sim_free(tmb_sim *s)
{
    tmb_islands_free(&s->islands);
    tmb_recorder_free(&s->recorder);
}

static void one_step(tmb_sim *s)
{
    tmb_world_step(s->world, s->opts.fixed_dt);
    s->steps++;

    if (s->opts.enable_sleeping) {
        if (tmb_islands_begin(&s->islands, s->world)) {
            tmb_islands_build(&s->islands, s->world);
            s->sleeping = tmb_islands_update_sleep(&s->islands, s->world,
                                                   s->opts.fixed_dt);
        }
    }
    if (s->opts.enable_profiling) {
        tmb_profile_add(&s->profile, "contacts",
                        (double)s->world->contact_count);
        tmb_profile_add(&s->profile, "bodies",
                        (double)tmb_world_body_count(s->world));
        tmb_profile_add(&s->profile, "sleeping", (double)s->sleeping);
    }
    if (s->opts.enable_recording)
        tmb_recorder_capture(&s->recorder, s->world);
}

int tmb_sim_advance(tmb_sim *s, float frame_dt)
{
    /* drive the fixed-step accumulator ourselves so each fixed step also runs
     * the subsystem updates */
    if (frame_dt < 0)
        frame_dt = 0;
    float max_frame = s->ts.fixed_dt * (float)s->ts.max_steps;
    if (frame_dt > max_frame)
        frame_dt = max_frame;
    s->ts.accumulator += frame_dt;
    int steps = 0;
    while (s->ts.accumulator >= s->ts.fixed_dt && steps < s->ts.max_steps) {
        one_step(s);
        s->ts.accumulator -= s->ts.fixed_dt;
        steps++;
    }
    s->ts.alpha = s->ts.fixed_dt > 0 ? s->ts.accumulator / s->ts.fixed_dt : 0;
    if (s->ts.alpha > 1)
        s->ts.alpha = 1;
    return steps;
}

void tmb_sim_run(tmb_sim *s, int count)
{
    for (int i = 0; i < count; i++)
        one_step(s);
}
