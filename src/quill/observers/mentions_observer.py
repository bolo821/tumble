"""MentionObserver: record @-mention events."""
from __future__ import annotations


class MentionObserver:
    def __init__(self):
        self._events = []

    def record(self, *, comment_id, mentioned_id, at):
        self._events.append({
            "comment_id": comment_id, "mentioned_id": mentioned_id, "at": at,
        })
        return self

    def for_mentioned(self, principal_id):
        return [e for e in self._events if e["mentioned_id"] == principal_id]

    def per_reviewer_counts(self):
        out = {}
        for e in self._events:
            out[e["mentioned_id"]] = out.get(e["mentioned_id"], 0) + 1
        return out

    def __len__(self):
        return len(self._events)
