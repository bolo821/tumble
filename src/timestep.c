#include "tumble/timestep.h"

void tmb_timestep_init(tmb_timestep *ts, float fixed_dt)
{
    ts->fixed_dt = fixed_dt > 0 ? fixed_dt : 1.0f / 60.0f;
    ts->accumulator = 0;
    ts->alpha = 0;
    ts->max_steps = 8;
    ts->total_steps = 0;
}

int tmb_timestep_advance(tmb_timestep *ts, tmb_world *w, float frame_dt)
{
    if (frame_dt < 0)
        frame_dt = 0;
    /* clamp a huge frame (e.g. after a breakpoint) to avoid runaway catch-up */
    float max_frame = ts->fixed_dt * (float)ts->max_steps;
    if (frame_dt > max_frame)
        frame_dt = max_frame;
    ts->accumulator += frame_dt;

    int steps = 0;
    while (ts->accumulator >= ts->fixed_dt && steps < ts->max_steps) {
        tmb_world_step(w, ts->fixed_dt);
        ts->accumulator -= ts->fixed_dt;
        steps++;
        ts->total_steps++;
    }
    ts->alpha = ts->fixed_dt > 0 ? ts->accumulator / ts->fixed_dt : 0;
    if (ts->alpha < 0)
        ts->alpha = 0;
    else if (ts->alpha > 1)
        ts->alpha = 1;
    return steps;
}
