"""rename: track file renames across a review."""
from __future__ import annotations

from quill.files.file_change import RENAMED


def renames_for_review(store, review_id):
    return [c for c in store.for_review(review_id) if c.kind == RENAMED]


def rename_pairs(store, review_id):
    return [(c.old_path, c.path) for c in renames_for_review(store, review_id)]


def follow_rename(store, review_id, *, original_path):
    for c in renames_for_review(store, review_id):
        if c.old_path == original_path:
            return c.path
    return original_path


def original_of(store, review_id, *, current_path):
    for c in renames_for_review(store, review_id):
        if c.path == current_path:
            return c.old_path
    return current_path
