"""backoff: compute wait times."""
from __future__ import annotations


def linear(*, attempt, step_seconds):
    return attempt * step_seconds


def exponential(*, attempt, base_seconds, cap_seconds=0):
    delay = base_seconds * (2 ** max(0, attempt - 1))
    if cap_seconds:
        return min(delay, cap_seconds)
    return delay


def fixed(*, seconds):
    return seconds


def jittered(*, attempt, base_seconds, fraction):
    return int(base_seconds + base_seconds * fraction * attempt)
