"""hard_kill: cancel expired checks."""
from __future__ import annotations


class KillSink:
    def __init__(self):
        self._killed = []

    def kill(self, *, check_id, at, reason="timeout"):
        self._killed.append({"check_id": check_id, "at": at, "reason": reason})
        return self

    def killed_ids(self):
        return sorted({k["check_id"] for k in self._killed})

    def for_check(self, check_id):
        return [k for k in self._killed if k["check_id"] == check_id]

    def __len__(self):
        return len(self._killed)


def kill_expired(*, tracker, sink, now):
    for cid in tracker.expired_checks(now=now):
        sink.kill(check_id=cid, at=now)
        tracker.stop(cid)
    return sink
