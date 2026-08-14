from quill.core.thread import Thread
from quill.threads.store import ThreadStore
from quill.threads.resolver import resolve, unresolve, resolve_all_for_review, unresolved_count


def test_resolve():
    t = Thread(id="t", review_id="r")
    resolve(t)
    assert t.resolved
    unresolve(t)
    assert not t.resolved


def test_resolve_all_for_review():
    s = ThreadStore()
    s.add(Thread(id="t1", review_id="r"))
    s.add(Thread(id="t2", review_id="r"))
    changed = resolve_all_for_review(s, review_id="r")
    assert set(changed) == {"t1", "t2"}
    assert unresolved_count(s, "r") == 0
