from quill.core.review import Review
from quill.search.facets import by_author, by_status, by_label


def test_by_author():
    reviews = [
        Review(id="r1", title="t", author="alice"),
        Review(id="r2", title="t", author="alice"),
        Review(id="r3", title="t", author="bob"),
    ]
    facets = by_author(reviews)
    assert facets == {"alice": 2, "bob": 1}


def test_by_status():
    reviews = [
        Review(id="r", title="t", author="a", status="open"),
        Review(id="r2", title="t", author="a", status="merged"),
    ]
    facets = by_status(reviews)
    assert facets["open"] == 1
    assert facets["merged"] == 1


def test_by_label():
    reviews = [
        Review(id="r", title="t", author="a", labels=("bug", "urgent")),
        Review(id="r2", title="t", author="a", labels=("bug",)),
    ]
    facets = by_label(reviews)
    assert facets == {"bug": 2, "urgent": 1}
