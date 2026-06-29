# quill

A small code-review platform library.

quill gives you a vocabulary for describing pull requests, review comments,
reviewer assignments, and merge policies -- then drives a review through its
lifecycle: draft -> open -> approved -> merged | closed. Around that
lifecycle there are threads, suggested changes, required-reviewer groups, a
merge queue, notifications, indexes, audit trails, snapshots, and search.

## install

```bash
pip install -e .
```

## quick start

```python
from quill import Review, Comment, ReviewStore

store = ReviewStore()
review = Review(id="r1", title="Add rate limiting", author="alice")
store.add(review)
```

quill tries to be small, readable, and obvious about the moving parts (the
review status FSM, the required-reviewer resolution, the merge-queue admit
rule) so each one can be reasoned about in isolation.
