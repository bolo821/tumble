"""diff: compute differences."""
from __future__ import annotations


def added(desired, actual):
    actual_ids = {a.id for a in actual}
    return [d for d in desired if d.id not in actual_ids]


def removed(desired, actual):
    desired_ids = {d.id for d in desired}
    return [a for a in actual if a.id not in desired_ids]


def common(desired, actual):
    desired_ids = {d.id for d in desired}
    return [a for a in actual if a.id in desired_ids]


def diff_count(desired, actual):
    return len(added(desired, actual)) + len(removed(desired, actual))


def is_converged(desired, actual):
    return diff_count(desired, actual) == 0
