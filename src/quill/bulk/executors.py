"""executors: per-kind BulkOp executors."""
from __future__ import annotations


def label_executor(op, review):
    params = op.parameter_dict()
    label = params.get("label", "")
    if label and not review.has_label(label):
        review.labels = tuple(list(review.labels) + [label])


def unlabel_executor(op, review):
    params = op.parameter_dict()
    label = params.get("label", "")
    if label and review.has_label(label):
        review.labels = tuple(l for l in review.labels if l != label)


def assign_executor(op, review):
    """No-op stub; assignment goes through AssignmentStore separately."""
    return


def close_executor(op, review):
    from quill.core.status import ReviewStatus
    if not review.is_terminal():
        review.status = ReviewStatus.CLOSED
