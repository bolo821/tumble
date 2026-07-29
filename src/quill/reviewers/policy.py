"""policy: assignment policies (round-robin / least-loaded)."""
from __future__ import annotations


def pick_round_robin(*, rotation, pool_key, candidates):
    return rotation.next(pool_key=pool_key, candidates=candidates)


def pick_least_loaded(*, assignment_store, candidates):
    from quill.reviewers.load import least_loaded
    reviewers = [r for r in candidates]
    if not reviewers:
        return None
    return least_loaded(assignment_store, reviewers)


def pick_by_group(*, registry, group):
    return registry.with_group(group)


def merge_with_codeowners(*, initial_reviewers, codeowners_for_paths):
    out = set(initial_reviewers)
    for owners in codeowners_for_paths:
        out.update(owners)
    return sorted(out)
