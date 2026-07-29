"""projection: project quill types to compact dicts."""
from __future__ import annotations


def project_review(review, *, fields=("id", "status")):
    return {f: getattr(review, f, None) for f in fields}


def project_comment(comment, *, include_body=False):
    out = {
        "id": comment.id, "review_id": comment.review_id,
        "author": comment.author, "resolved": comment.resolved,
    }
    if include_body:
        out["body"] = comment.body
    return out


def project_thread(thread):
    return {
        "id": thread.id, "review_id": thread.review_id,
        "resolved": thread.resolved,
    }


def projects_many(items, *, project_fn):
    return [project_fn(item) for item in items]
