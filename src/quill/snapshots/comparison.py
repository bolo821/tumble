"""comparison: deeper snapshot comparisons."""
from __future__ import annotations


def shared_review_ids(a, b):
    a_ids = {r["id"] for r in a.review_payload}
    b_ids = {r["id"] for r in b.review_payload}
    return sorted(a_ids & b_ids)


def divergent_status(a, b):
    by_a = {r["id"]: r["status"] for r in a.review_payload}
    by_b = {r["id"]: r["status"] for r in b.review_payload}
    out = []
    for rid in shared_review_ids(a, b):
        if by_a[rid] != by_b[rid]:
            out.append({"id": rid, "a": by_a[rid], "b": by_b[rid]})
    return out


def review_count_difference(a, b):
    return b.review_count() - a.review_count()


def jaccard_review_ids(a, b):
    a_ids = {r["id"] for r in a.review_payload}
    b_ids = {r["id"] for r in b.review_payload}
    if not a_ids and not b_ids:
        return 0.0
    return len(a_ids & b_ids) / len(a_ids | b_ids)
