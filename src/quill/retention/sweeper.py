"""sweeper: sweep reviews needing expiration."""
from __future__ import annotations

from quill.retention.decider import must_expire


def sweep_expirable(*, store, registry, now):
    return sorted(r.id for r in store.all()
                  if must_expire(review=r, registry=registry, now=now))


def expirable_count(*, store, registry, now):
    return len(sweep_expirable(store=store, registry=registry, now=now))
