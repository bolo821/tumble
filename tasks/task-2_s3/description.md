# Description — Quadtree Clear/Reuse Use-After-Free

## Background

`tmb_quadtree` is a point-region quadtree. A node holds up to
`TMB_QT_NODE_CAP` items; on overflow it subdivides into four children
(`kids[0..3]`) and sets `divided = 1`. Traversals (`query_node`,
`insert_node`, `depth_of`) recurse into `kids[]` only when `divided` is set.

To support per-frame reuse, `tmb_quadtree_clear` empties the tree without
releasing the root:

```c
void tmb_quadtree_clear(tmb_quadtree *q)
{
    if (!q || !q->root)
        return;
    for (int i = 0; i < 4; i++)
        free_node(q->root->kids[i]); /* frees child subtrees */
    q->root->count = 0;
    q->size = 0;
}
```

## Root cause

`free_node` releases the child subtrees, but `tmb_quadtree_clear` never
clears the state that points at them:

- `q->root->divided` stays `1`, and
- `q->root->kids[0..3]` still hold the addresses of the just-freed nodes.

The root is now a *divided* node whose child pointers are dangling. The very
next traversal trusts `divided` and recurses:

```c
if (n->divided)
    for (int i = 0; i < 4; i++)
        query_node(n->kids[i], region, out, cap, count); /* freed node */
```

`query_node` then reads `n->bounds` / `n->count` / `n->divided` from freed
memory — a heap use-after-free. (An insert after the clear can hit the same
dangling pointers; a second free would additionally double-free.)

The flaw is easy to miss because `clear` looks like it "resets" the tree —
it empties the item count and frees the children — but forgets that the
`divided`/`kids` state is what every traversal keys off of.

## Trigger

`fuzz/quadtree_fuzzer.c` inserts a batch of spread-out points (forcing the
root to subdivide), calls `tmb_quadtree_clear`, inserts a couple more
points, then queries. `poc.bin` is 24 points (48 bytes, all in
`[0x01, 0x7f]`); after the clear the region query dereferences the freed
child nodes and ASan reports heap-use-after-free in `query_node`.

## Fix

Node lifetime is unified under a single owner: a `tmb_pool` block allocator
stored on the tree. Every node — root and children — is carved from the
pool (`tmb_pool_calloc`, added so a fresh node starts zeroed), and:

- `tmb_quadtree_clear` releases the child subtrees back to the pool *and*
  turns each parent back into a leaf (`divided = 0`, `kids[i] = NULL`), so no
  traversal can follow a released node;
- `tmb_quadtree_free` simply frees the pool, reclaiming all nodes at once
  with no per-node walk that could double-free;
- `subdivide` allocates children from the pool and rolls back cleanly on
  partial allocation failure.

Because the tree never retains a pointer to a released node, no input can
drive a use-after-free through the clear/reuse path. `tests/test_quadtree.c`
covers clear-then-query, repeated clear cycles, shallow clears, and query
correctness after a clear.
