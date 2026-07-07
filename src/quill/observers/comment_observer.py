"""CommentObserver: record comment activity."""
from __future__ import annotations


class CommentObserver:
    def __init__(self):
        self._events = []

    def record_post(self, *, comment_id, review_id, author, at):
        self._events.append({
            "kind": "post", "comment_id": comment_id,
            "review_id": review_id, "author": author, "at": at,
        })
        return self

    def record_resolve(self, *, comment_id, at, actor=""):
        self._events.append({
            "kind": "resolve", "comment_id": comment_id,
            "at": at, "actor": actor,
        })
        return self

    def events(self):
        return list(self._events)

    def for_comment(self, comment_id):
        return [e for e in self._events if e.get("comment_id") == comment_id]

    def by_author(self, author):
        return [e for e in self._events if e.get("author") == author]

    def __len__(self):
        return len(self._events)
