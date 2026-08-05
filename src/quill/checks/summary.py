"""summary: per-review check-suite summary."""
from __future__ import annotations

from quill.checks.check_run import SUCCESS, FAILURE, PENDING


def suite_status(store, review_id):
    checks = store.for_review(review_id)
    if not checks:
        return "no-checks"
    if any(c.status == FAILURE for c in checks):
        return "failing"
    if any(c.status == PENDING for c in checks):
        return "pending"
    if all(c.status == SUCCESS for c in checks):
        return "passing"
    return "mixed"


def status_summary(store, review_id):
    return {
        "suite_status": suite_status(store, review_id),
        "total": len(store.for_review(review_id)),
    }
