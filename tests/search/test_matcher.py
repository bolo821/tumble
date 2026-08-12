from quill.core.review import Review
from quill.search.query import Query
from quill.search.matcher import matches, filter_reviews


def test_empty_query_matches_all():
    r = Review(id="r", title="t", author="a")
    assert matches(r, Query())


def test_title_substring():
    r = Review(id="r", title="Add rate limiting", author="a")
    assert matches(r, Query(title_substring="rate"))
    assert not matches(r, Query(title_substring="delete"))


def test_authors_filter():
    r = Review(id="r", title="t", author="alice")
    assert matches(r, Query(authors=("alice",)))
    assert not matches(r, Query(authors=("bob",)))


def test_labels_all_requires_all():
    r = Review(id="r", title="t", author="a", labels=("a", "b"))
    assert matches(r, Query(labels_all=("a", "b")))
    assert not matches(r, Query(labels_all=("a", "c")))


def test_filter_reviews():
    reviews = [
        Review(id="r1", title="t", author="alice"),
        Review(id="r2", title="t", author="bob"),
    ]
    assert len(filter_reviews(reviews, Query(authors=("alice",)))) == 1
