"""recorder: build a Snapshot from a ReviewStore."""
from __future__ import annotations

from quill.snapshots.snapshot import Snapshot


def from_store(store, *, at):
    payload = tuple(
        {"id": r.id, "status": r.status, "author": r.author}
        for r in store.all()
    )
    return Snapshot(at=at, review_payload=payload)


def record(snap_store, *, review_store, at):
    snap = from_store(review_store, at=at)
    snap_store.record(snap)
    return snap


def from_reviews(reviews, *, at):
    payload = tuple(
        {"id": r.id, "status": r.status, "author": r.author}
        for r in reviews
    )
    return Snapshot(at=at, review_payload=payload)
