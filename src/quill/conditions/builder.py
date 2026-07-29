"""builder: fluent WhenClause builder."""
from __future__ import annotations

from quill.conditions.when_clause import WhenClause


class WhenBuilder:
    def __init__(self):
        self._expression = ""

    def equals(self, *, attr, value):
        self._expression = attr + ' == "' + value + '"'
        return self

    def not_equals(self, *, attr, value):
        self._expression = attr + ' != "' + value + '"'
        return self

    def has_attribute(self, name):
        self._expression = name
        return self

    def lacks_attribute(self, name):
        self._expression = "not " + name
        return self

    def always(self):
        self._expression = ""
        return self

    def never(self):
        self._expression = "never"
        return self

    def build(self):
        return WhenClause(expression=self._expression)
