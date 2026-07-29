"""reorder: reorder items in a MergeQueue."""
from __future__ import annotations


def promote(queue, *, review_id):
    if review_id not in queue._entries:
        return False
    queue._entries.remove(review_id)
    queue._entries.insert(0, review_id)
    return True


def demote(queue, *, review_id):
    if review_id not in queue._entries:
        return False
    queue._entries.remove(review_id)
    queue._entries.append(review_id)
    return True


def swap_positions(queue, *, a, b):
    try:
        ia = queue._entries.index(a)
        ib = queue._entries.index(b)
    except ValueError:
        return False
    queue._entries[ia], queue._entries[ib] = queue._entries[ib], queue._entries[ia]
    return True


def reverse(queue):
    queue._entries.reverse()
    return queue
