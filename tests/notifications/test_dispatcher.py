from quill.notifications.channel import Channel
from quill.notifications.router import Router
from quill.notifications.dispatcher import Dispatcher


def test_dispatch_sends_to_all_applicable():
    r = Router()
    r.register(Channel(name="email", kind="email", target="t", on_open=True))
    r.register(Channel(name="slack", kind="slack", target="#x", on_open=True))
    d = Dispatcher()
    received = []
    d.dispatch(router=r, event_kind="open", review_id="r1",
               sink=lambda c, rid: received.append((c.name, rid)))
    assert len(received) == 2
    assert d.sent_count() == 2
