"""group: per-group principal membership."""
from __future__ import annotations


class GroupStore:
    def __init__(self):
        self._members = {}

    def add(self, *, group, principal_id):
        self._members.setdefault(group, set()).add(principal_id)
        return self

    def remove(self, *, group, principal_id):
        if group in self._members:
            self._members[group].discard(principal_id)
        return self

    def members(self, group):
        return sorted(self._members.get(group, set()))

    def groups_for(self, principal_id):
        return sorted(g for g, m in self._members.items() if principal_id in m)

    def groups(self):
        return sorted(self._members)

    def __len__(self):
        return len(self._members)


def is_member(store, *, group, principal_id):
    return principal_id in store._members.get(group, set())
