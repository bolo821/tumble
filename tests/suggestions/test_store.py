from quill.suggestions.suggestion import Suggestion
from quill.suggestions.store import SuggestionStore


def test_add_and_query():
    store = SuggestionStore()
    s = Suggestion(id="s1", comment_id="c1", review_id="r1",
                   file_path="x.py",
                   original_lines=("a",), proposed_lines=("b",))
    store.add(s)
    assert store.get("s1") is s
    assert store.for_review("r1") == [s]
    assert store.for_comment("c1") == [s]
    assert len(store.pending()) == 1
