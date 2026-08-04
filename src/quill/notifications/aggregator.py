"""aggregator: per-Dispatcher aggregates."""
from __future__ import annotations


def per_channel_counts(dispatcher):
    out = {}
    for e in dispatcher._sent:
        out[e["channel"]] = out.get(e["channel"], 0) + 1
    return out


def per_event_counts(dispatcher):
    out = {}
    for e in dispatcher._sent:
        out[e["event"]] = out.get(e["event"], 0) + 1
    return out


def most_active_channel(dispatcher):
    counts = per_channel_counts(dispatcher)
    if not counts:
        return None
    return max(counts.items(), key=lambda kv: kv[1])[0]


def top_channels(dispatcher, *, n=5):
    counts = per_channel_counts(dispatcher)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]
