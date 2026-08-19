from quill.branches.branch import Branch, BranchRegistry


def test_register_and_get():
    reg = BranchRegistry()
    reg.register(Branch(name="main", is_protected=True))
    assert reg.get("main").is_protected
    assert reg.protected()[0].name == "main"


def test_names():
    reg = BranchRegistry()
    reg.register(Branch(name="main"))
    reg.register(Branch(name="dev"))
    assert reg.names() == ["dev", "main"]
