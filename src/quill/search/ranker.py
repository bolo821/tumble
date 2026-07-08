"""ranker: simple score-based ranking."""
from __future__ import annotations


def base_score(review, q):
    score = 0
    if q.title_substring and q.title_substring.lower() in review.title.lower():
        score += 10
    if q.labels_any:
        score += len(set(q.labels_any) & review.label_set()) * 3
    if review.is_open():
        score += 2
    return score


def rank(reviews, q):
    scored = [(base_score(r, q), r) for r in reviews]
    scored.sort(key=lambda kv: (-kv[0], kv[1].id))
    return [r for _, r in scored]


def top_n(reviews, q, *, n=10):
    return rank(reviews, q)[:n]
