from quill.core.review import Review
from quill.core.status import ReviewStatus
from quill.search.query import Query
from quill.search.ranker import rank, top_n


def test_title_match_boosts_score():
    a = Review(id="a", title="Add rate limiting", author="a",
               status=ReviewStatus.OPEN)
    b = Review(id="b", title="Refactor logger", author="a")
    ranked = rank([b, a], Query(title_substring="rate"))
    assert ranked[0].id == "a"


def test_top_n_limits():
    reviews = [Review(id=str(i), title="t", author="a") for i in range(10)]
    assert len(top_n(reviews, Query(), n=3)) == 3
