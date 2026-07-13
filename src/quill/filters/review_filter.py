"""review_filter: predicates over Reviews."""
from __future__ import annotations


def by_status(status):
    def pred(review):
        return review.status == status
    return pred


def by_author(author):
    def pred(review):
        return review.author == author
    return pred


def by_base_branch(branch):
    def pred(review):
        return review.base_branch == branch
    return pred


def has_label(label):
    def pred(review):
        return review.has_label(label)
    return pred


def title_contains(substring):
    def pred(review):
        return substring.lower() in review.title.lower()
    return pred


def apply_all(predicates, reviews):
    out = list(reviews)
    for pred in predicates:
        out = [r for r in out if pred(r)]
    return out


def apply_any(predicates, reviews):
    return [r for r in reviews if any(pred(r) for pred in predicates)]
