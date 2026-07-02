"""policy: gate merge-queue admission."""
from __future__ import annotations

from quill.core.status import ReviewStatus


def is_admittable(review, *, is_approved, ci_green, has_conflicts):
    if review.status != ReviewStatus.APPROVED:
        return False
    if not is_approved:
        return False
    if not ci_green:
        return False
    if has_conflicts:
        return False
    return True


def admit_reason(*, is_approved, ci_green, has_conflicts):
    if not is_approved:
        return "not-approved"
    if not ci_green:
        return "ci-not-green"
    if has_conflicts:
        return "conflicts"
    return "admittable"
