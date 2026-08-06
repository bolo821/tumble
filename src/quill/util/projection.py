"""projection: project quill objects into compact dicts."""
from __future__ import annotations


def project_review(review, *, fields=("id", "status")):
    return {f: getattr(review, f, None) for f in fields}


def project_comment(comment, *, fields=("id", "author")):
    return {f: getattr(comment, f, None) for f in fields}


def project_thread(thread, *, fields=("id", "resolved")):
    return {f: getattr(thread, f, None) for f in fields}


def projects_many(items, *, project_fn):
    return [project_fn(item) for item in items]
