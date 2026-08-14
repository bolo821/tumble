from quill.observability.histogram import Histogram


def test_observe_and_average():
    h = Histogram(buckets=[10, 20, 30])
    for v in [5, 15, 25, 35]:
        h.observe(v)
    assert h.total_count() == 4
    assert h.average() == 20


def test_bucket_counts():
    h = Histogram(buckets=[10])
    h.observe(5)
    h.observe(15)
    assert h.bucket_counts() == [1, 1]
