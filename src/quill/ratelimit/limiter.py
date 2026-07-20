"""RateLimiter: per-key rolling window limiter."""
from __future__ import annotations


class RateLimiter:
    def __init__(self, *, capacity, window_seconds):
        self._capacity = capacity
        self._window = window_seconds
        self._events = {}

    def allow(self, *, key, at):
        events = [t for t in self._events.get(key, []) if t > at - self._window]
        if len(events) >= self._capacity:
            self._events[key] = events
            return False
        events.append(at)
        self._events[key] = events
        return True

    def count(self, *, key, at):
        events = [t for t in self._events.get(key, []) if t > at - self._window]
        self._events[key] = events
        return len(events)

    def remaining(self, *, key, at):
        return max(0, self._capacity - self.count(key=key, at=at))

    def keys(self):
        return sorted(self._events)
