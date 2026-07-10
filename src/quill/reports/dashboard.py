"""dashboard: aggregate rollup."""
from __future__ import annotations


def per_author(store):
    out = {}
    for r in store.all():
        out[r.author] = out.get(r.author, 0) + 1
    return out


def per_status(store):
    out = {}
    for r in store.all():
        out[r.status] = out.get(r.status, 0) + 1
    return out


def render(store, *, width=60):
    out = ["=" * width]
    out.append("DASHBOARD".center(width))
    out.append("=" * width)
    out.append("total reviews: " + str(len(store)))
    for status, count in sorted(per_status(store).items()):
        out.append(status.ljust(15) + str(count))
    return "\n".join(out)
