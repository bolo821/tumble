"""policy_evaluator: unified policy evaluation for review admission."""
from __future__ import annotations

from quill.approvals.policy import is_satisfied
from quill.checks.policy import is_all_green
from quill.merge_queue.policy import is_admittable


def can_merge(*, review, approval_log, required_approvals, check_store,
              required_checks, has_conflicts):
    if not is_satisfied(log=approval_log, review_id=review.id,
                        required_count=required_approvals):
        return False, "not-enough-approvals"
    if not is_all_green(store=check_store, review_id=review.id,
                        required_names=required_checks):
        return False, "checks-not-green"
    if not is_admittable(review, is_approved=True, ci_green=True,
                         has_conflicts=has_conflicts):
        return False, "not-admittable"
    return True, "ready-to-merge"
