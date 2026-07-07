"""composite: cross-index queries."""
from __future__ import annotations


def reviews_in_status_by_author(by_status, by_author, *, status, author):
    a = set(by_status.find(status))
    b = set(by_author.find(author))
    return sorted(a & b)


def reviews_in_status_touching_file(by_status, by_file, *, status, path):
    a = set(by_status.find(status))
    b = set(by_file.find(path))
    return sorted(a & b)


def reviews_by_reviewer_and_status(by_reviewer, by_status, *, reviewer, status):
    a = set(by_reviewer.find(reviewer))
    b = set(by_status.find(status))
    return sorted(a & b)


def reviews_by_author_touching_file(by_author, by_file, *, author, path):
    a = set(by_author.find(author))
    b = set(by_file.find(path))
    return sorted(a & b)
