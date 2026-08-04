"""registry: per-comment mention bookkeeping."""
from __future__ import annotations

from quill.mentions.parser import parse


class MentionRegistry:
    def __init__(self):
        self._by_comment = {}
        self._by_principal = {}

    def index(self, comment):
        mentions = parse(comment.body)
        self._by_comment[comment.id] = mentions
        for m in mentions:
            self._by_principal.setdefault(m, set()).add(comment.id)
        return self

    def for_comment(self, comment_id):
        return list(self._by_comment.get(comment_id, []))

    def comments_mentioning(self, principal_id):
        return sorted(self._by_principal.get(principal_id, set()))

    def mention_count(self, principal_id):
        return len(self._by_principal.get(principal_id, set()))

    def all_mentioned(self):
        return sorted(self._by_principal)
