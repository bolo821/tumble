from quill.queue.priority_queue import PriorityWorkQueue


def test_priority_order():
    q = PriorityWorkQueue()
    q.enqueue("later", priority=10)
    q.enqueue("sooner", priority=1)
    q.enqueue("middle", priority=5)
    assert q.dequeue() == "sooner"
    assert q.dequeue() == "middle"
    assert q.dequeue() == "later"


def test_stable_within_priority():
    q = PriorityWorkQueue()
    q.enqueue("a", priority=1)
    q.enqueue("b", priority=1)
    assert q.dequeue() == "a"
    assert q.dequeue() == "b"


def test_items_at():
    q = PriorityWorkQueue()
    q.enqueue("a", priority=1)
    q.enqueue("b", priority=1)
    q.enqueue("c", priority=2)
    assert q.items_at(1) == ["a", "b"]
