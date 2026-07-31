"""aggregator: per-reviewer aggregates."""
from __future__ import annotations


def per_reviewer_approval_counts(log):
    out = {}
    for a in log.all():
        if a.is_approve():
            out[a.reviewer] = out.get(a.reviewer, 0) + 1
    return out


def per_reviewer_change_request_counts(log):
    out = {}
    for a in log.all():
        if a.is_request_changes():
            out[a.reviewer] = out.get(a.reviewer, 0) + 1
    return out


def most_positive_reviewers(log, *, n=5):
    counts = per_reviewer_approval_counts(log)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]


def most_critical_reviewers(log, *, n=5):
    counts = per_reviewer_change_request_counts(log)
    return sorted(counts.items(), key=lambda kv: -kv[1])[:n]
