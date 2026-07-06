"""policy: required-check evaluation."""
from __future__ import annotations

from quill.checks.check_run import SUCCESS, FAILURE


def is_all_green(*, store, review_id, required_names):
    for name in required_names:
        latest = store.latest_for(review_id, name=name)
        if latest is None:
            return False
        if latest.status != SUCCESS:
            return False
    return True


def failing_checks(*, store, review_id):
    out = []
    for r in store.for_review(review_id):
        if r.status == FAILURE:
            out.append(r.name)
    return sorted(set(out))


def pending_checks(*, store, review_id):
    from quill.checks.check_run import PENDING
    return sorted({r.name for r in store.for_review(review_id)
                   if r.status == PENDING})


def missing_required(*, store, review_id, required_names):
    return sorted(n for n in required_names
                  if store.latest_for(review_id, name=n) is None)
