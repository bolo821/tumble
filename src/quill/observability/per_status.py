"""per_status: per-status counter helpers."""
from __future__ import annotations


def increment_for(counter, *, status, by=1):
    counter.inc(key="status:" + status, by=by)
    return counter


def count_for(counter, *, status):
    return counter.get("status:" + status)


def all_statuses(counter):
    return sorted(k[7:] for k in counter.keys() if k.startswith("status:"))


def total_across_statuses(counter):
    return sum(counter.get(k) for k in counter.keys() if k.startswith("status:"))
