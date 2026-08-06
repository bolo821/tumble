"""query: helper queries over SegmentStore."""
from __future__ import annotations


def segments_containing(store, *, review_id):
    out = set()
    for segment_name in store.segments():
        if review_id in store.members(segment_name):
            out.add(segment_name)
    return sorted(out)


def is_in(store, *, segment_name, review_id):
    return review_id in store.members(segment_name)


def largest_segments(store, *, n=5):
    sized = [(name, store.size(name)) for name in store.segments()]
    sized.sort(key=lambda kv: -kv[1])
    return sized[:n]


def total_memberships(store):
    return sum(store.size(name) for name in store.segments())
