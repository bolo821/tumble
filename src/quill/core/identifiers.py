"""Helpers for composing quill identifiers."""
from __future__ import annotations


def review_id(*, repo, sequence):
    return repo + "#" + str(sequence)


def comment_id(*, review_id_, sequence):
    return review_id_ + ":comment:" + str(sequence)


def thread_id(*, review_id_, sequence):
    return review_id_ + ":thread:" + str(sequence)


def parse_review_id(s):
    repo, _, seq = s.rpartition("#")
    return repo, int(seq) if seq else 0
