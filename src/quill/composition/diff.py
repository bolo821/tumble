"""diff: differences between two ReviewStores."""
from __future__ import annotations


def added(before, after):
    before_ids = {r.id for r in before.all()}
    return [r for r in after.all() if r.id not in before_ids]


def removed(before, after):
    after_ids = {r.id for r in after.all()}
    return [r for r in before.all() if r.id not in after_ids]


def changed(before, after):
    before_by_id = {r.id: r for r in before.all()}
    out = []
    for r in after.all():
        prev = before_by_id.get(r.id)
        if prev is None:
            continue
        if (prev.status != r.status or prev.additions != r.additions
                or prev.deletions != r.deletions):
            out.append((prev, r))
    return out


def is_quiet(before, after):
    return (not added(before, after) and not removed(before, after)
            and not changed(before, after))
