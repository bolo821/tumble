"""TokenStore: per-id token storage."""
from __future__ import annotations


class TokenStore:
    def __init__(self):
        self._by_id = {}

    def issue(self, token):
        self._by_id[token.id] = token
        return self

    def get(self, token_id):
        return self._by_id.get(token_id)

    def revoke(self, token_id):
        self._by_id.pop(token_id, None)
        return self

    def all(self):
        return list(self._by_id.values())

    def for_principal(self, principal_id):
        return [t for t in self._by_id.values() if t.principal_id == principal_id]

    def with_scope(self, scope):
        return [t for t in self._by_id.values() if t.scope == scope]

    def active(self, *, now):
        return [t for t in self._by_id.values() if not t.is_expired(now=now)]

    def __len__(self):
        return len(self._by_id)
