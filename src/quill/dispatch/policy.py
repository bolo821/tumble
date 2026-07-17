"""policy: dispatch admission policies."""
from __future__ import annotations


def admit_unless_paused(*, is_paused):
    if is_paused:
        return False, "paused"
    return True, "admitted"


def admit_under_capacity(*, in_flight, capacity):
    if in_flight >= capacity:
        return False, "no-capacity"
    return True, "admitted"


def admit_outside_maintenance(*, in_maintenance):
    if in_maintenance:
        return False, "maintenance"
    return True, "admitted"


def combined(*decisions):
    for ok, reason in decisions:
        if not ok:
            return False, reason
    return True, "admitted"
