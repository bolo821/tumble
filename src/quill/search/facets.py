"""facets: per-attribute facet counts."""
from __future__ import annotations


def by_author(reviews):
    out = {}
    for r in reviews:
        out[r.author] = out.get(r.author, 0) + 1
    return out


def by_status(reviews):
    out = {}
    for r in reviews:
        out[r.status] = out.get(r.status, 0) + 1
    return out


def by_label(reviews):
    out = {}
    for r in reviews:
        for label in r.label_set():
            out[label] = out.get(label, 0) + 1
    return out
