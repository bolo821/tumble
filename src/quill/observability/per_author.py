"""per_author: per-author counter helpers."""
from __future__ import annotations


def increment_for(counter, *, author, by=1):
    counter.inc(key="author:" + author, by=by)
    return counter


def count_for(counter, *, author):
    return counter.get("author:" + author)


def all_authors(counter):
    return sorted(k[7:] for k in counter.keys() if k.startswith("author:"))


def total_across_authors(counter):
    return sum(counter.get(k) for k in counter.keys() if k.startswith("author:"))
