from quill.retention.policy import RetentionPolicy, PolicyRegistry


def test_must_expire_by():
    p = RetentionPolicy(label="tmp", keep_seconds=100)
    assert p.must_expire_by(created_at=10) == 110


def test_registry_get():
    reg = PolicyRegistry()
    p = RetentionPolicy(label="tmp", keep_seconds=100)
    reg.register(p)
    assert reg.get("tmp") is p
    assert reg.labels() == ["tmp"]
