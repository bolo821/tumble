"""Transcript: per-review rolling transcript."""
from __future__ import annotations


class Transcript:
    def __init__(self):
        self._lines = []

    def append(self, *, at, level, message):
        self._lines.append({"at": at, "level": level, "message": message})
        return self

    def info(self, *, at, message):
        return self.append(at=at, level="info", message=message)

    def warn(self, *, at, message):
        return self.append(at=at, level="warn", message=message)

    def error(self, *, at, message):
        return self.append(at=at, level="error", message=message)

    def lines(self):
        return list(self._lines)

    def at_level(self, level):
        return [l for l in self._lines if l["level"] == level]

    def in_window(self, *, start, end):
        return [l for l in self._lines if start <= l["at"] < end]

    def __len__(self):
        return len(self._lines)
