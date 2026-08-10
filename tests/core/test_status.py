from quill.core.status import ReviewStatus


def test_terminal_set():
    assert ReviewStatus.is_terminal(ReviewStatus.MERGED)
    assert ReviewStatus.is_terminal(ReviewStatus.CLOSED)
    assert not ReviewStatus.is_terminal(ReviewStatus.OPEN)


def test_active_set():
    for s in (ReviewStatus.DRAFT, ReviewStatus.OPEN, ReviewStatus.APPROVED):
        assert ReviewStatus.is_active(s)
    for s in (ReviewStatus.MERGED, ReviewStatus.CLOSED):
        assert not ReviewStatus.is_active(s)


def test_is_mergeable():
    assert ReviewStatus.is_mergeable(ReviewStatus.APPROVED)
    assert not ReviewStatus.is_mergeable(ReviewStatus.OPEN)
