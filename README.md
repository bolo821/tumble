# Tumble

Tumble is a small, dependency-free 2D rigid-body physics engine written in
portable C11. It is built for games, demos and tools that need believable
contact physics without pulling in a large framework: a few thousand lines of
plain C, no allocations on the hot path beyond growable scratch buffers, and a
clean separation between the engine core and the optional subsystems layered
around it.

## Features

- Circle and convex-polygon shapes with computed mass and inertia.
- Broadphase by n-body sweep, sweep-and-prune, or a dynamic AABB tree (BVH).
- SAT narrowphase with two-point manifolds and a sequential-impulse solver.
- Distance and revolute joints, plus weld/prismatic/motor/mouse/pulley
  constraints.
- Islands and sleeping, fixed-timestep stepping and render interpolation.
- Extras: GJK distance, raycasting, circle casts, time-of-impact, force fields,
  wind, buoyancy, conveyors, steering, particles and springs.
- Text and binary scene formats with defensive, bounds-checked loaders.
- ASCII, SVG and JSON exporters; a CLI front end.

## Building

```sh
make            # static library + the `tumble` CLI
make test       # build and run the unit tests
make examples   # build the example programs
```

Or with CMake:

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

## Using the library

```c
#include "tumble/tumble.h"

tmb_world w;
tmb_world_init(&w);
w.gravity = tmb_v2(0, -9.8f);

tmb_body ground;
tmb_body_init(&ground, TMB_BODY_STATIC, tmb_shape_box(20, 0.5f), tmb_v2(0, -5));
tmb_world_add_body(&w, &ground);

tmb_body ball;
tmb_body_init(&ball, TMB_BODY_DYNAMIC, tmb_shape_circle(0.5f), tmb_v2(0, 5));
tmb_world_add_body(&w, &ball);

for (int i = 0; i < 120; i++)
    tmb_world_step(&w, 1.0f / 60.0f);

tmb_world_free(&w);
```

The loaders fill a world you have already initialised with `tmb_world_init`:

```c
tmb_world w;
tmb_world_init(&w);
tmb_load_file("scene.tmb", &w);
```

## CLI

```sh
tumble run scene.tmb 300        # simulate and print final body states
tumble render scene.tmb 60 24   # ASCII snapshot
tumble gen pit out.tmb          # generate a procedural scene
tumble convert in.tmb out.bin   # text <-> binary
```

## Scene format

See [docs/FORMATS.md](docs/FORMATS.md) for the text and binary scene formats and
[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for how a step is structured.

## Fuzzing

The `fuzz/` targets drive the scene loaders and the simulation step under
libFuzzer; see `.clusterfuzzlite/` for the ClusterFuzzLite build.

## License

MIT. See [LICENSE](LICENSE).
