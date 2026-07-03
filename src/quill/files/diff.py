"""diff: helpers over per-review Hunk sets."""
from __future__ import annotations


def total_hunks(hunks):
    return len(list(hunks))


def hunks_for_file(hunks, path):
    return [h for h in hunks if h.file_path == path]


def touched_files(hunks):
    return sorted({h.file_path for h in hunks})


def total_line_delta(hunks):
    return sum(h.line_delta() for h in hunks)


def touches_line(hunks, *, file_path, line):
    for h in hunks_for_file(hunks, file_path):
        start, end = h.new_range()
        if start <= line < end:
            return True
    return False
