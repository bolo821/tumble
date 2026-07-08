"""aggregator: per-actor / per-kind rollups."""
from __future__ import annotations


def total_by_actor(log):
    out = {}
    for e in log.events():
        out[e["actor"]] = out.get(e["actor"], 0) + 1
    return out


def total_by_kind(log):
    out = {}
    for e in log.events():
        out[e["kind"]] = out.get(e["kind"], 0) + 1
    return out


def total_by_target(log):
    out = {}
    for e in log.events():
        out[e["target"]] = out.get(e["target"], 0) + 1
    return out


def top_actors(log, *, n=5):
    counts = total_by_actor(log)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def top_kinds(log, *, n=5):
    counts = total_by_kind(log)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]
