"""emitters: high-level observation emitters."""
from __future__ import annotations


def emit_review_created(counter):
    counter.inc(key="review.created")
    return counter


def emit_review_merged(counter):
    counter.inc(key="review.merged")
    return counter


def emit_review_closed(counter):
    counter.inc(key="review.closed")
    return counter


def emit_comment_posted(counter):
    counter.inc(key="comment.posted")
    return counter


def emit_approval_recorded(counter, *, decision):
    counter.inc(key="approval." + decision)
    return counter


def emit_merge_queue_depth(gauge, *, depth):
    gauge.set(key="merge_queue.depth", value=depth)
    return gauge
