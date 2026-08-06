"""view: convenience over SuggestionStore."""
from __future__ import annotations


def pending_for_review(store, review_id):
    return [s for s in store.for_review(review_id) if s.is_pending()]


def applied_for_review(store, review_id):
    return [s for s in store.for_review(review_id) if s.is_applied()]


def total_files_touched(store, review_id):
    return sorted({s.file_path for s in store.for_review(review_id)})


def suggestions_by_file(store, review_id):
    out = {}
    for s in store.for_review(review_id):
        out.setdefault(s.file_path, []).append(s)
    return out
