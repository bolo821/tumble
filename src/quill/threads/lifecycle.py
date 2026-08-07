"""lifecycle: helpers for creating and closing threads."""
from __future__ import annotations

from quill.core.thread import Thread


def open_thread(store, *, thread_id, review_id, file_path="", line=0, at=0):
    thread = Thread(id=thread_id, review_id=review_id,
                    file_path=file_path, line=line,
                    resolved=False, created_at=at)
    store.add(thread)
    return thread


def close_thread(store, *, thread_id):
    thread = store.get(thread_id)
    if thread is None:
        return False
    thread.resolved = True
    return True


def reopen_thread(store, *, thread_id):
    thread = store.get(thread_id)
    if thread is None:
        return False
    thread.resolved = False
    return True
