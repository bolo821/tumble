"""scheduler: produce micro-batches from a stream."""
from __future__ import annotations


def split_into_batches(items, *, size):
    items = list(items)
    return [items[i:i + size] for i in range(0, len(items), size)]


def total_batches(items, *, size):
    n = len(list(items))
    if size <= 0:
        return 0
    return (n + size - 1) // size


def items_in_batch(items, *, size, index):
    items = list(items)
    start = index * size
    return items[start:start + size]
