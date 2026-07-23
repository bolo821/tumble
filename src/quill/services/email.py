"""EmailService: outbound email bookkeeping."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Email:
    recipient: str
    subject: str
    body: str
    at: int


class EmailService:
    def __init__(self):
        self._sent = []

    def send(self, email):
        self._sent.append(email)
        return self

    def sent(self):
        return list(self._sent)

    def for_recipient(self, recipient):
        return [e for e in self._sent if e.recipient == recipient]

    def with_subject_substring(self, substr):
        return [e for e in self._sent if substr in e.subject]

    def total_sent(self):
        return len(self._sent)
