from quill.branches.protection import ProtectionRule, ProtectionRegistry, required_approvals, required_checks


def _registry():
    reg = ProtectionRegistry()
    reg.register(ProtectionRule(branch="main", required_approvals=2,
                                 required_checks=("ci", "lint")))
    return reg


def test_required_approvals():
    reg = _registry()
    assert required_approvals(reg, branch="main") == 2
    assert required_approvals(reg, branch="other", default=1) == 1


def test_required_checks():
    reg = _registry()
    assert required_checks(reg, branch="main") == ["ci", "lint"]
    assert required_checks(reg, branch="other") == []
