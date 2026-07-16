"""comment_encoder: encode/decode a Comment."""
from __future__ import annotations

from quill.core.comment import Comment


def encode(comment):
    return {
        "id": comment.id, "review_id": comment.review_id,
        "thread_id": comment.thread_id,
        "author": comment.author, "body": comment.body,
        "file_path": comment.file_path, "line": comment.line,
        "created_at": comment.created_at, "resolved": comment.resolved,
        "is_suggestion": comment.is_suggestion,
    }


def decode(d):
    return Comment(
        id=d["id"], review_id=d["review_id"], thread_id=d["thread_id"],
        author=d["author"], body=d["body"],
        file_path=d.get("file_path", ""), line=d.get("line", 0),
        created_at=d.get("created_at", 0),
        resolved=d.get("resolved", False),
        is_suggestion=d.get("is_suggestion", False),
    )


def encode_many(items):
    return [encode(c) for c in items]


def decode_many(records):
    return [decode(r) for r in records]
