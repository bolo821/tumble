"""Hunk: a contiguous run of changes within a file."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Hunk:
    file_path: str
    old_start: int
    old_lines: int
    new_start: int
    new_lines: int
    body: str = ""

    def line_delta(self):
        return self.new_lines - self.old_lines

    def old_range(self):
        return (self.old_start, self.old_start + self.old_lines)

    def new_range(self):
        return (self.new_start, self.new_start + self.new_lines)
