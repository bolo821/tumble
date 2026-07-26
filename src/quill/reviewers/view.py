"""view: view queries over ReviewerRegistry."""
from __future__ import annotations


def by_prefix(registry, prefix):
    return [r for r in registry.all() if r.id.startswith(prefix)]


def display_names(registry):
    return sorted(r.short_label() for r in registry.all())


def total_bots(registry):
    return len(registry.bots())


def total_humans(registry):
    return len(registry.humans())


def in_any_group(registry, groups):
    candidates = set(groups)
    return [r for r in registry.all() if r.group_set() & candidates]
