"""thread_lint: structural checks on a Thread."""
from __future__ import annotations


def lint(thread):
    issues = []
    if not thread.id:
        issues.append("missing thread id")
    if not thread.review_id:
        issues.append("thread lacks review_id")
    if thread.file_path and thread.line <= 0:
        issues.append("inline thread " + thread.id + " has non-positive line")
    return issues


def is_valid(thread):
    return not lint(thread)
