"""dispatcher: send notification via a sink."""
from __future__ import annotations


class Dispatcher:
    def __init__(self):
        self._sent = []

    def dispatch(self, *, router, event_kind, review_id, sink, at=0):
        recipients = router.channels_for(event_kind)
        for c in recipients:
            sink(c, review_id)
            self._sent.append({"channel": c.name, "review_id": review_id,
                               "event": event_kind, "at": at})
        return len(recipients)

    def sent_count(self):
        return len(self._sent)

    def for_review(self, review_id):
        return [s for s in self._sent if s["review_id"] == review_id]

    def for_channel(self, name):
        return [s for s in self._sent if s["channel"] == name]
