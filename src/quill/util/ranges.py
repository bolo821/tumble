"""ranges: integer-range helpers."""
from __future__ import annotations


def overlap(a_start, a_end, b_start, b_end):
    return max(0, min(a_end, b_end) - max(a_start, b_start))


def contains(outer_start, outer_end, inner_start, inner_end):
    return outer_start <= inner_start and inner_end <= outer_end


def union_length(intervals):
    if not intervals:
        return 0
    merged = [tuple(sorted(intervals)[0])]
    for start, end in sorted(intervals)[1:]:
        last_start, last_end = merged[-1]
        if start <= last_end:
            merged[-1] = (last_start, max(last_end, end))
        else:
            merged.append((start, end))
    return sum(e - s for s, e in merged)


def split_at(start, end, *, points):
    points = sorted({p for p in points if start < p < end})
    out = []
    cursor = start
    for p in points:
        out.append((cursor, p))
        cursor = p
    out.append((cursor, end))
    return out
