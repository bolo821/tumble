"""diff: compare two snapshots."""
from __future__ import annotations


def added_ids(before, after):
    a_ids = {r["id"] for r in before.review_payload}
    return [r["id"] for r in after.review_payload if r["id"] not in a_ids]


def removed_ids(before, after):
    b_ids = {r["id"] for r in after.review_payload}
    return [r["id"] for r in before.review_payload if r["id"] not in b_ids]


def status_changed(before, after):
    by_id = {r["id"]: r["status"] for r in before.review_payload}
    out = []
    for r in after.review_payload:
        prev = by_id.get(r["id"])
        if prev is not None and prev != r["status"]:
            out.append({"id": r["id"], "from": prev, "to": r["status"]})
    return out


def count_delta(before, after):
    return after.review_count() - before.review_count()


def is_quiet(before, after):
    return (not added_ids(before, after) and not removed_ids(before, after)
            and not status_changed(before, after))
