"""review_lint: structural checks on a Review."""
from __future__ import annotations


def lint(review):
    issues = []
    if not review.id:
        issues.append("missing review id")
    if not review.title:
        issues.append("missing review title")
    if not review.author:
        issues.append("missing author")
    if not review.base_branch:
        issues.append("missing base_branch")
    if review.additions < 0:
        issues.append("negative additions")
    if review.deletions < 0:
        issues.append("negative deletions")
    return issues


def is_valid(review):
    return not lint(review)
