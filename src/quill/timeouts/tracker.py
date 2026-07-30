"""TimeoutTracker: per-check timeout bookkeeping."""
from __future__ import annotations


class TimeoutTracker:
    def __init__(self):
        self._starts = {}
        self._timeouts = {}

    def start(self, *, check_id, at, timeout_seconds):
        self._starts[check_id] = at
        self._timeouts[check_id] = timeout_seconds
        return self

    def stop(self, check_id):
        self._starts.pop(check_id, None)
        self._timeouts.pop(check_id, None)
        return self

    def is_expired(self, *, check_id, now):
        start = self._starts.get(check_id)
        if start is None:
            return False
        timeout = self._timeouts.get(check_id, 0)
        if timeout <= 0:
            return False
        return (now - start) >= timeout

    def time_remaining(self, *, check_id, now):
        start = self._starts.get(check_id)
        if start is None:
            return 0
        timeout = self._timeouts.get(check_id, 0)
        if timeout <= 0:
            return 0
        return max(0, timeout - (now - start))

    def expired_checks(self, *, now):
        return [cid for cid in self._starts if self.is_expired(check_id=cid, now=now)]

    def active_checks(self):
        return sorted(self._starts.keys())
