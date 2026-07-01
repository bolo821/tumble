# Description — Joint Body-Pointer Cache Use-After-Free After Compact

## Background

`tmb_joints_prepare` resolves each joint's bodies by index on the first call
(when `body_a_ptr == NULL`) and then caches the resolved pointer for fast
access on subsequent calls:

```c
/* start of prepare: use cache if available */
tmb_body *A = j->body_a_ptr ? j->body_a_ptr : &w->bodies[j->a];
...
/* end of prepare: populate cache for next call */
j->body_a_ptr = &w->bodies[j->a];
j->body_b_ptr = &w->bodies[j->b];
```

## Root cause

`tmb_world_remove_body` compacts the body array by calling
`realloc(w->bodies, body_count * sizeof(*nb))`.  Under ASan (and in
practice when the allocator needs to relocate), `realloc` returns a new
address and marks the old allocation as freed.

Any `body_a_ptr` or `body_b_ptr` set during a prior step now points into
the freed (poisoned) block.  The very next `tmb_joints_prepare` call reads
`j->body_a_ptr->vel`, `->pos`, etc. from that freed memory → heap-use-after-free.

The bug requires the removal of a **bystander** body (not a joint endpoint):
removing a joint endpoint would require joint eviction (a separate concern),
whereas removing a bystander leaves the joint intact but shifts the
allocation address.

## Fix

`tmb_joints_invalidate_ptr_cache(w)` is called at the end of
`tmb_world_remove_body`, immediately after the compact realloc:

```c
tmb_joints_invalidate_ptr_cache(w);
```

It iterates the joint table and sets `body_a_ptr = body_b_ptr = NULL`.
The next `tmb_joints_prepare` call then falls back to the safe
`&w->bodies[j->a]` index-based lookup, re-populating the cache from the
new allocation address.

A `tmb_joints_ptr_cache_valid` diagnostic is wired into the fuzzer harness
remove operation so regressions are caught immediately after any compact.

## Trigger sequence (decoded from poc.bin)

1. Add three dynamic boxes (bodies 1, 2, 3).  Bodies 1 and 2 are joint
   endpoints; body 3 is a bystander.
2. Add a distance joint between bodies 1 and 2.
3. Step once — `tmb_joints_prepare` sets `body_a_ptr = &bodies[1]` and
   `body_b_ptr = &bodies[2]`.
4. Remove body 3 (bystander) — compact realloc frees the old allocation;
   `body_a_ptr` and `body_b_ptr` now point into freed memory.
5. Step again — `tmb_joints_prepare` dereferences `j->body_a_ptr` →
   AddressSanitizer: heap-use-after-free.
