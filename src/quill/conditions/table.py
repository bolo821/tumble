"""ConditionTable: row-based decision table."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ConditionRow:
    name: str
    conditions: tuple
    decision: str

    def matches(self, context):
        for attr, expected in self.conditions:
            if context.get(attr) != expected:
                return False
        return True


class ConditionTable:
    def __init__(self):
        self._rows = []

    def add_row(self, row):
        self._rows.append(row)
        return self

    def evaluate(self, context):
        for row in self._rows:
            if row.matches(context):
                return row.decision
        return None

    def matching_rows(self, context):
        return [row for row in self._rows if row.matches(context)]

    def row_count(self):
        return len(self._rows)
