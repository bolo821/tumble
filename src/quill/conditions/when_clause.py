"""WhenClause: simple boolean expression."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class WhenClause:
    expression: str = ""

    def is_always(self):
        return self.expression == ""

    def is_never(self):
        return self.expression == "never"


def parse(expression):
    return WhenClause(expression=expression)
