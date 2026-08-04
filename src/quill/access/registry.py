"""AclRegistry: per-namespace Acl bookkeeping."""
from __future__ import annotations

from quill.access.acl import Acl


class AclRegistry:
    def __init__(self):
        self._by_namespace = {}

    def get_or_create(self, namespace):
        if namespace not in self._by_namespace:
            self._by_namespace[namespace] = Acl()
        return self._by_namespace[namespace]

    def get(self, namespace):
        return self._by_namespace.get(namespace)

    def namespaces(self):
        return sorted(self._by_namespace)

    def total_entries(self):
        return sum(len(acl) for acl in self._by_namespace.values())

    def __len__(self):
        return len(self._by_namespace)
