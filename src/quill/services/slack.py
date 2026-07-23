"""SlackService: outbound Slack messages."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class SlackMessage:
    channel: str
    text: str
    at: int


class SlackService:
    def __init__(self):
        self._messages = []

    def post(self, message):
        self._messages.append(message)
        return self

    def messages(self):
        return list(self._messages)

    def for_channel(self, channel):
        return [m for m in self._messages if m.channel == channel]

    def with_substring(self, substr):
        return [m for m in self._messages if substr in m.text]

    def __len__(self):
        return len(self._messages)
