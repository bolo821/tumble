"""aggregator: aggregate over SuggestionStore."""
from __future__ import annotations


def per_status(store):
    out = {}
    for s in store.all():
        out[s.status] = out.get(s.status, 0) + 1
    return out


def total_line_delta(store):
    return sum(s.line_delta() for s in store.all())


def per_file_count(store):
    out = {}
    for s in store.all():
        out[s.file_path] = out.get(s.file_path, 0) + 1
    return out


def applied_files(store):
    return sorted({s.file_path for s in store.applied()})


def most_suggested_files(store, *, n=5):
    counts = per_file_count(store)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]
