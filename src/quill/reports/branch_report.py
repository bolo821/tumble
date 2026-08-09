"""branch_report: per-base-branch review summary."""
from __future__ import annotations


def per_branch_counts(store):
    out = {}
    for r in store.all():
        out[r.base_branch] = out.get(r.base_branch, 0) + 1
    return out


def per_branch_status_counts(store, *, branch):
    out = {}
    for r in store.all():
        if r.base_branch != branch:
            continue
        out[r.status] = out.get(r.status, 0) + 1
    return out


def render(store, *, width=60):
    out = ["=" * width]
    out.append("BRANCH REPORT".center(width))
    out.append("=" * width)
    for branch, count in sorted(per_branch_counts(store).items()):
        out.append("  " + branch.ljust(20) + str(count))
    return "\n".join(out)
