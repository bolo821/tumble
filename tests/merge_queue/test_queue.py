from quill.merge_queue.queue import MergeQueue


def test_enqueue_and_position():
    q = MergeQueue()
    assert q.enqueue("r1")
    assert q.enqueue("r2")
    assert q.position("r1") == 0
    assert q.position("r2") == 1


def test_enqueue_dedup():
    q = MergeQueue()
    q.enqueue("r1")
    assert not q.enqueue("r1")
    assert q.depth() == 1


def test_dequeue():
    q = MergeQueue()
    q.enqueue("r1")
    q.enqueue("r2")
    assert q.dequeue() == "r1"
    assert q.depth() == 1


def test_remove():
    q = MergeQueue()
    q.enqueue("r1")
    q.enqueue("r2")
    assert q.remove("r1")
    assert q.ids() == ["r2"]


def test_position_not_found():
    q = MergeQueue()
    assert q.position("missing") == -1
