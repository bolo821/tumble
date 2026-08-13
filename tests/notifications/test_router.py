from quill.notifications.channel import Channel
from quill.notifications.router import Router


def test_router_routes_on_open():
    r = Router()
    r.register(Channel(name="c", kind="email", target="t@example.com",
                       on_open=True, on_merge=False))
    assert len(r.channels_for("open")) == 1
    assert len(r.channels_for("merge")) == 0


def test_for_kind():
    r = Router()
    r.register(Channel(name="c1", kind="email", target="t"))
    r.register(Channel(name="c2", kind="slack", target="#x"))
    assert len(r.for_kind("email")) == 1
