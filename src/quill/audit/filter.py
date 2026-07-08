"""filter: filter audit log by predicates."""
from __future__ import annotations


def by_kind_set(log, kinds):
    s = set(kinds)
    return [e for e in log.events() if e["kind"] in s]


def by_actor_set(log, actors):
    s = set(actors)
    return [e for e in log.events() if e["actor"] in s]


def by_target_prefix(log, prefix):
    return [e for e in log.events() if e["target"].startswith(prefix)]


def by_window_and_kind(log, *, kinds, start, end):
    s = set(kinds)
    out = []
    for e in log.events():
        if e["kind"] in s and start <= e["at"] < end:
            out.append(e)
    return out
