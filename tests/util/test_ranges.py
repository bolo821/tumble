from quill.util.ranges import overlap, contains, union_length, split_at


def test_overlap():
    assert overlap(0, 10, 5, 15) == 5
    assert overlap(0, 5, 5, 10) == 0


def test_contains():
    assert contains(0, 100, 25, 50)
    assert not contains(0, 100, -1, 50)


def test_union_length():
    assert union_length([(0, 5), (3, 8), (10, 15)]) == 13


def test_split_at():
    assert split_at(0, 10, points=[3, 7]) == [(0, 3), (3, 7), (7, 10)]
