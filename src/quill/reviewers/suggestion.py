"""suggestion: recommend reviewers for a review."""
from __future__ import annotations

from quill.authors.expertise import authors_for_path


def by_touched_paths(store, *, paths, exclude=()):
    out = set()
    for path in paths:
        out.update(authors_for_path(store, path))
    for e in exclude:
        out.discard(e)
    return sorted(out)


def by_recent_reviews(assignment_store, *, review_ids, exclude=()):
    out = set()
    for rid in review_ids:
        for reviewer in assignment_store.for_review(rid):
            out.add(reviewer)
    for e in exclude:
        out.discard(e)
    return sorted(out)


def merged(paths_reviewers, recent_reviewers):
    out = set(paths_reviewers) | set(recent_reviewers)
    return sorted(out)
