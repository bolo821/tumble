"""observer_view: read-only view of QueueObserver."""
from __future__ import annotations


def enqueue_events(observer):
    return [e for e in observer._events if e["kind"] == "enqueue"]


def dequeue_events(observer):
    return [e for e in observer._events if e["kind"] == "dequeue"]


def total_events(observer):
    return len(observer._events)


def net_queue_change(observer):
    return observer.enqueue_count() - observer.dequeue_count()
