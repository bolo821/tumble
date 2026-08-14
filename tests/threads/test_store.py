from quill.core.thread import Thread
from quill.threads.store import ThreadStore


def test_add_and_query():
    s = ThreadStore()
    s.add(Thread(id="t1", review_id="r1", file_path="a.py", line=10))
    s.add(Thread(id="t2", review_id="r1", resolved=True))
    assert s.get("t1").file_path == "a.py"
    assert len(s.for_review("r1")) == 2
    assert len(s.inline("r1")) == 1
    assert len(s.general("r1")) == 1
    assert len(s.resolved("r1")) == 1
    assert len(s.unresolved("r1")) == 1
