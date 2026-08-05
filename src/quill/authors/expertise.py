"""expertise: infer author expertise from touched paths."""
from __future__ import annotations


def paths_touched_by(store, author):
    out = set()
    for r in store.all():
        if r.author == author:
            out.update(r.changed_files)
    return sorted(out)


def paths_shared_between(store, *, a, b):
    return sorted(set(paths_touched_by(store, a)) & set(paths_touched_by(store, b)))


def top_paths_for(store, author, *, n=5):
    counts = {}
    for r in store.all():
        if r.author != author:
            continue
        for p in r.changed_files:
            counts[p] = counts.get(p, 0) + 1
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def authors_for_path(store, path):
    out = set()
    for r in store.all():
        if path in r.changed_files:
            out.add(r.author)
    return sorted(out)
