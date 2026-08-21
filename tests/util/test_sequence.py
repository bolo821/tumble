from quill.util.sequence import first_or_none, last_or_none, all_equal, count_matching, find_first, find_last


def test_first_or_none():
    assert first_or_none([1, 2, 3]) == 1
    assert first_or_none([]) is None


def test_last_or_none():
    assert last_or_none([1, 2, 3]) == 3
    assert last_or_none([]) is None


def test_all_equal():
    assert all_equal([1, 1, 1])
    assert not all_equal([1, 2])
    assert all_equal([])


def test_count_matching():
    assert count_matching([1, 2, 3, 4], lambda x: x % 2 == 0) == 2


def test_find_first():
    assert find_first([1, 2, 3, 4], lambda x: x > 2) == 3


def test_find_last():
    assert find_last([1, 2, 3, 4], lambda x: x < 4) == 3
