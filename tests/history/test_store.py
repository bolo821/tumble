from quill.history.store import HistoryStore


def test_append_and_query():
    s = HistoryStore()
    s.append({"review_id": "r1", "at": 10, "kind": "open", "actor": "alice"})
    s.append({"review_id": "r2", "at": 20, "kind": "open", "actor": "bob"})
    assert len(s) == 2
    assert s.for_review("r1")[0]["actor"] == "alice"


def test_latest_n():
    s = HistoryStore()
    for i in range(5):
        s.append({"review_id": "r", "at": i, "kind": "x", "actor": "a"})
    assert len(s.latest(n=2)) == 2
