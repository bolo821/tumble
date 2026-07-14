"""cohort: per-cohort analysis."""
from __future__ import annotations


def by_author(reviews):
    out = {}
    for r in reviews:
        out.setdefault(r.author, []).append(r)
    return out


def by_base_branch(reviews):
    out = {}
    for r in reviews:
        out.setdefault(r.base_branch, []).append(r)
    return out


def by_status(reviews):
    out = {}
    for r in reviews:
        out.setdefault(r.status, []).append(r)
    return out


def cohort_count(grouped):
    return {k: len(v) for k, v in grouped.items()}


def cohort_summaries(grouped):
    return {k: {"count": len(v)} for k, v in grouped.items()}
