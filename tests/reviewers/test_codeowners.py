from quill.reviewers.codeowners import CodeownerRegistry, CodeownerRule


def _registry():
    reg = CodeownerRegistry()
    reg.add(CodeownerRule(pattern="*", owners=("@team",)))
    reg.add(CodeownerRule(pattern="src/api/*", owners=("api-team",)))
    return reg


def test_later_rules_win():
    reg = _registry()
    assert reg.owners_for("src/api/x.py") == ["api-team"]
    assert reg.owners_for("README.md") == ["@team"]


def test_all_owners():
    reg = _registry()
    assert reg.all_owners() == ["@team", "api-team"]
