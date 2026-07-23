"""WebhookService: outbound webhook bookkeeping."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class WebhookPost:
    url: str
    payload: dict
    at: int
    status_code: int = 200


class WebhookService:
    def __init__(self):
        self._posts = []

    def record(self, post):
        self._posts.append(post)
        return self

    def posts(self):
        return list(self._posts)

    def for_url(self, url):
        return [p for p in self._posts if p.url == url]

    def successful(self):
        return [p for p in self._posts if 200 <= p.status_code < 300]

    def failures(self):
        return [p for p in self._posts if not (200 <= p.status_code < 300)]

    def __len__(self):
        return len(self._posts)
