"""diff: compare two audit windows."""
from __future__ import annotations

from quill.audit.aggregator import total_by_kind, total_by_actor


def kind_delta(before, after):
    a = total_by_kind(before)
    b = total_by_kind(after)
    keys = set(a) | set(b)
    return {k: b.get(k, 0) - a.get(k, 0) for k in keys}


def actor_delta(before, after):
    a = total_by_actor(before)
    b = total_by_actor(after)
    keys = set(a) | set(b)
    return {k: b.get(k, 0) - a.get(k, 0) for k in keys}


def new_actors(before, after):
    a = {e["actor"] for e in before.events()}
    b = {e["actor"] for e in after.events()}
    return sorted(b - a)


def departed_actors(before, after):
    a = {e["actor"] for e in before.events()}
    b = {e["actor"] for e in after.events()}
    return sorted(a - b)
