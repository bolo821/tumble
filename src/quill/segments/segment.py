"""Segment: a labeled subset."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Segment:
    name: str
    description: str = ""


class SegmentStore:
    def __init__(self):
        self._segments = {}
        self._memberships = {}

    def declare(self, segment):
        self._segments[segment.name] = segment
        self._memberships.setdefault(segment.name, set())
        return self

    def add(self, *, segment_name, review_id):
        self._memberships.setdefault(segment_name, set()).add(review_id)
        return self

    def remove(self, *, segment_name, review_id):
        if segment_name in self._memberships:
            self._memberships[segment_name].discard(review_id)
        return self

    def members(self, segment_name):
        return sorted(self._memberships.get(segment_name, set()))

    def size(self, segment_name):
        return len(self._memberships.get(segment_name, set()))

    def segments(self):
        return sorted(self._segments)
