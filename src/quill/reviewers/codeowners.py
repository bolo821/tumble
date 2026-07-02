"""codeowners: per-path required-reviewer rules."""
from __future__ import annotations

from dataclasses import dataclass
from fnmatch import fnmatchcase


@dataclass(frozen=True)
class CodeownerRule:
    pattern: str
    owners: tuple  # tuple of reviewer_ids OR group names starting with @


class CodeownerRegistry:
    def __init__(self):
        self._rules = []

    def add(self, rule):
        self._rules.append(rule)
        return self

    def owners_for(self, path):
        """Later rules take precedence over earlier ones."""
        winning = ()
        for rule in self._rules:
            if fnmatchcase(path, rule.pattern):
                winning = rule.owners
        return list(winning)

    def rules(self):
        return list(self._rules)

    def all_owners(self):
        out = set()
        for r in self._rules:
            out.update(r.owners)
        return sorted(out)
