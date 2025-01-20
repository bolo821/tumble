# Architecture

## A step

`tmb_world_step(world, dt)` runs the pipeline:

1. **Sync** cached transforms from each body's position and angle.
2. **Integrate forces** — apply gravity and accumulated forces to velocities.
3. **Broadphase** — find candidate overlapping pairs (n-body sweep by default;
   sweep-and-prune and the AABB tree are available as alternatives).
4. **Narrowphase** — for each pair, run the circle/polygon collision routines
   and emit up to two contact points per manifold into the world contact list.
5. **Solve velocities** — sequential impulses over the contacts and joints for
   `velocity_iters` iterations.
6. **Integrate velocities** — advance positions.
7. **Solve positions** — Baumgarte/positional correction for `position_iters`
   iterations.
8. **Clear forces** for the next step.

## Memory

The world owns three growable arrays — bodies, joints and the per-step contact
list. The contact list is rebuilt every step: its count resets to zero while
its capacity is retained, and `tmb_contact_alloc` grows it (via `realloc`) when
a step produces more contacts than the current capacity. Callers that retain a
pointer into the contact array across an allocation must re-fetch it, because a
growth can move the array.

## Layers

The core (`shape`, `body`, `world`, `collide`, `solver`, broadphases, loaders)
is self-contained. Everything else — islands, sleeping, force fields, wind,
buoyancy, conveyors, steering, recorder, exporters — is an optional layer that
operates on a world through the public API and can be ignored if unused.
