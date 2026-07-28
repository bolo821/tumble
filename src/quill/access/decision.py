"""decision: structured access decisions."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class AccessDecision:
    decision: str  # grant | deny
    reason: str
    scope: str = ""

    def is_grant(self):
        return self.decision == "grant"

    def is_deny(self):
        return self.decision == "deny"


def grant(*, scope="", reason=""):
    return AccessDecision(decision="grant", reason=reason, scope=scope)


def deny(*, reason=""):
    return AccessDecision(decision="deny", reason=reason)
