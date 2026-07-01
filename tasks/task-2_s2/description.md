# Description — Extended Joint Stale Body Index After Compact

## Background

`tmb_xjoint` (weld / spring / mouse) stores `uint32_t a, b` — the indices
of the two bodies it connects.  Indices are resolved at each step inside
`tmb_xjoint_prepare`, which reads `&w->bodies[xj->a]` and `&w->bodies[xj->b]`.

## Root cause

`tmb_world_remove_body` performs a swap-pop: the last body (index `last`) is
moved into the freed slot (index `i`), then the array is compacted to
`body_count` elements via `realloc`.  Before this fix, no code remapped the
joint and xjoint tables:

- Any xjoint with `a == last` or `b == last` now pointed to index `last`
  which no longer exists in the compacted array (it would be index `i` after
  the swap).
- On the next step, `tmb_xjoint_prepare` computed `&w->bodies[last]` with
  `last >= body_count` → one-past-end or further OOB read.

## Fix

`tmb_xjoints_on_remove(w, removed, moved)` is called before the compact
realloc.  It walks the xjoint table and:

1. Removes any xjoint whose `a` or `b` equals `removed` (swap-pop within
   the xjoint table itself).
2. For surviving xjoints where `a == moved` or `b == moved`, rewrites the
   index to `removed` (the new location of the moved body).

The same treatment is applied to the standard `tmb_joint` table via
`tmb_joints_on_remove`.  A `tmb_xjoints_validate` helper is wired into the
fuzzer harness.

## Trigger sequence (decoded from poc.bin)

1. Add three dynamic boxes (bodies 1, 2, 3).
2. Add a weld xjoint between bodies 1 and 3.
3. Step once (xjoint is prepared; no crash yet).
4. Remove body 1 (swap-pop moves body 3 → index 1; xjoint.a still = 3).
5. Remove body 1 again (compacts; body 3 no longer exists at index 3).
6. Step — `tmb_xjoint_prepare` reads `w->bodies[3]` with only 2 bodies left
   → AddressSanitizer: heap-buffer-overflow.
