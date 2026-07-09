"""aggregate: roll-ups over event streams."""
from __future__ import annotations


def count_by_kind(stream):
    out = {}
    for e in stream.events():
        out[e["kind"]] = out.get(e["kind"], 0) + 1
    return out


def count_by_review(stream):
    out = {}
    for e in stream.events():
        out[e["review_id"]] = out.get(e["review_id"], 0) + 1
    return out


def first_event_for_review(stream, review_id):
    for e in stream.events():
        if e["review_id"] == review_id:
            return e
    return None


def last_event_for_review(stream, review_id):
    for e in reversed(stream.events()):
        if e["review_id"] == review_id:
            return e
    return None
