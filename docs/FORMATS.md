# Scene formats

Tumble reads two scene formats. The loaders fill a world that the caller has
already initialised with `tmb_world_init`; both cap body and joint counts at
`TMB_SCENE_MAX_BODIES` / `TMB_SCENE_MAX_JOINTS` so a small input cannot request
an enormous simulation.

## Text format

One directive per line; blank lines and unknown directives are ignored.

```
gravity GX GY
iters VEL POS
circle X Y R [static|dynamic] [density]
box X Y HW HH [ANGLE] [static|dynamic] [density]
poly X Y N x0 y0 x1 y1 ... [static|dynamic]
joint distance A B AX AY BX BY REST
joint revolute A B AX AY BX BY
```

- Coordinates and sizes are floats. Bodies default to dynamic.
- `poly` takes `N` local vertices, counter-clockwise; they are recentered on
  the body origin at `(X, Y)`.
- Joint body indices refer to the order bodies were declared (0-based).

Example:

```
gravity 0 -9.8
iters 8 3
box 0 -2 10 0.5 static
circle 0 4 0.5 dynamic
```

## Binary format

A little-endian binary container, recognised by its 4-byte magic `TMBB`:

| field            | type        | notes                          |
|------------------|-------------|--------------------------------|
| magic            | char[4]     | `TMBB`                         |
| version          | u16         | currently 1                    |
| gravity x, y     | f32, f32    |                                |
| velocity_iters   | u16         |                                |
| position_iters   | u16         |                                |
| body_count       | u32         | bounded                        |
| bodies           | body[]      | see below                      |
| joint_count      | u32         | bounded                        |
| joints           | joint[]     |                                |

Every multi-byte read is bounds-checked against the remaining input, so a
truncated or hostile file fails with `TMB_ERR_TRUNCATED` rather than reading out
of bounds.
