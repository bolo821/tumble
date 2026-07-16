"""review_encoder: encode/decode a Review."""
from __future__ import annotations

from quill.core.review import Review


def encode(review):
    return {
        "id": review.id,
        "title": review.title,
        "author": review.author,
        "base_branch": review.base_branch,
        "head_branch": review.head_branch,
        "status": review.status,
        "created_at": review.created_at,
        "updated_at": review.updated_at,
        "labels": list(review.labels),
        "changed_files": list(review.changed_files),
        "additions": review.additions,
        "deletions": review.deletions,
    }


def decode(d):
    return Review(
        id=d["id"], title=d["title"], author=d["author"],
        base_branch=d.get("base_branch", "main"),
        head_branch=d.get("head_branch", ""),
        status=d.get("status", "draft"),
        created_at=d.get("created_at", 0),
        updated_at=d.get("updated_at", 0),
        labels=tuple(d.get("labels", [])),
        changed_files=tuple(d.get("changed_files", [])),
        additions=d.get("additions", 0),
        deletions=d.get("deletions", 0),
    )


def encode_many(reviews):
    return [encode(r) for r in reviews]


def decode_many(records):
    return [decode(r) for r in records]
