# Task 2 (s3) — Quadtree Clear/Reuse Use-After-Free

## Summary

`tmb_quadtree_clear` empties a tree for per-frame reuse. When the tree has
subdivided, it frees the root's four child subtrees (`free_node(root->kids[i])`)
and resets `root->count`, but it leaves `root->divided == 1` and the
`root->kids[]` pointers still aimed at the freed nodes. The next traversal —
`tmb_quadtree_query` (or an insert that recurses) — follows those dangling
pointers into freed memory.

## Bug class

Use-after-free (CWE-416), heap read of a freed node.

## Harness

```
clang -fsanitize=address,undefined -fsanitize=fuzzer -O1 -g \
      -I include $(ls src/*.c | grep -v cli.c) \
      fuzz/quadtree_fuzzer.c -o quadtree_fuzzer -lm
./quadtree_fuzzer poc.bin
```

`fuzz/quadtree_fuzzer.c` reads pairs of input bytes as points, inserts them
(forcing the root to subdivide), calls `tmb_quadtree_clear`, inserts a couple
more points, then runs a region query.

## PoC

`poc.bin` — 48 bytes, all in `[0x01, 0x7f]`. 24 well-spread points that force
subdivision; after the clear, the query dereferences the freed child nodes.

## Crash location

`src/quadtree.c` — `query_node` (traversing `root->kids[i]` freed by
`tmb_quadtree_clear`). AddressSanitizer: heap-use-after-free.

## Fix branch

`task-2-fix-s3` (diff against `task-2-buggy-s3` is `fix.patch`).

## Files changed in fix

- `src/quadtree.c` — node memory owned by a block pool; `clear` releases child
  subtrees and resets each parent to a leaf (`divided=0`, `kids=NULL`)
- `include/tumble/quadtree.h` — add the node pool to `tmb_quadtree`
- `src/pool.c`, `include/tumble/pool.h` — `tmb_pool_calloc` (zeroed blocks)
- `tests/test_quadtree.c` (new) — clear/reuse regression tests
