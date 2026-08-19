from quill.queue.work_queue import WorkQueue


def test_enqueue_dequeue():
    q = WorkQueue()
    q.enqueue("a")
    q.enqueue("b")
    assert q.dequeue() == "a"
    assert q.dequeue() == "b"
    assert q.dequeue() is None


def test_depth_and_empty():
    q = WorkQueue()
    assert q.is_empty()
    q.enqueue("x")
    assert q.depth() == 1


def test_drain_all():
    q = WorkQueue()
    for x in "abc":
        q.enqueue(x)
    assert q.drain() == ["a", "b", "c"]
    assert q.depth() == 0


def test_drain_limited():
    q = WorkQueue()
    for x in "abcd":
        q.enqueue(x)
    assert q.drain(max_items=2) == ["a", "b"]
    assert q.depth() == 2
