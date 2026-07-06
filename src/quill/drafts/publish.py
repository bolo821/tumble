"""publish: convert DraftComment(s) into published Comment(s)."""
from __future__ import annotations

from quill.core.comment import Comment


def publish_one(draft, *, comment_id, thread_id, at):
    return Comment(
        id=comment_id,
        review_id=draft.review_id,
        thread_id=thread_id,
        author=draft.author,
        body=draft.body,
        file_path=draft.file_path,
        line=draft.line,
        created_at=at,
    )


def publish_all_for_author(store, *, author, comment_store, thread_id, at, id_prefix):
    published = []
    for i, draft in enumerate(list(store.for_author(author))):
        cid = id_prefix + str(i)
        comment = publish_one(draft, comment_id=cid, thread_id=thread_id, at=at)
        comment_store.add(comment)
        store.drop(draft.id)
        published.append(comment)
    return published
