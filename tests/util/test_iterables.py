from quill.util.iterables import chunked, unique, partition, group_by, count_by


def test_chunked():
    assert chunked([1, 2, 3, 4, 5], size=2) == [[1, 2], [3, 4], [5]]


def test_unique_preserves_order():
    assert unique([1, 2, 1, 3, 2, 4]) == [1, 2, 3, 4]


def test_partition():
    yes, no = partition([1, 2, 3, 4], lambda x: x % 2 == 0)
    assert yes == [2, 4]
    assert no == [1, 3]


def test_group_by():
    items = ["aa", "bb", "ccc"]
    grouped = group_by(items, len)
    assert sorted(grouped[2]) == ["aa", "bb"]


def test_count_by():
    assert count_by(["aa", "bb", "ccc"], len) == {2: 2, 3: 1}
