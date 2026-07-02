"""apply: mark a Suggestion applied / discarded."""
from __future__ import annotations

from quill.suggestions.suggestion import APPLIED, DISCARDED


def apply(suggestion, *, actor, at):
    if not suggestion.is_pending():
        return False
    suggestion.status = APPLIED
    suggestion.applied_by = actor
    suggestion.applied_at = at
    return True


def discard(suggestion, *, actor, at):
    if not suggestion.is_pending():
        return False
    suggestion.status = DISCARDED
    suggestion.applied_by = actor
    suggestion.applied_at = at
    return True


def bulk_apply(suggestions, *, actor, at):
    applied = []
    for s in suggestions:
        if apply(s, actor=actor, at=at):
            applied.append(s.id)
    return applied
