"""protection: branch-protection rules."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ProtectionRule:
    branch: str
    required_approvals: int = 1
    required_checks: tuple = ()
    dismiss_stale_reviews: bool = False
    require_up_to_date: bool = True


class ProtectionRegistry:
    def __init__(self):
        self._by_branch = {}

    def register(self, rule):
        self._by_branch[rule.branch] = rule
        return self

    def get(self, branch):
        return self._by_branch.get(branch)

    def all(self):
        return list(self._by_branch.values())

    def branches(self):
        return sorted(self._by_branch)


def required_approvals(registry, *, branch, default=1):
    rule = registry.get(branch)
    return rule.required_approvals if rule else default


def required_checks(registry, *, branch):
    rule = registry.get(branch)
    return list(rule.required_checks) if rule else []
