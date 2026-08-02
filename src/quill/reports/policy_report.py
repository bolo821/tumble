"""policy_report: per-branch protection summary."""
from __future__ import annotations

from quill.branches.protection import required_approvals, required_checks


def summary(registry, branches):
    out = []
    for branch in branches:
        out.append({
            "branch": branch,
            "required_approvals": required_approvals(registry, branch=branch),
            "required_checks": required_checks(registry, branch=branch),
        })
    return out


def render(rows, *, width=60):
    out = ["=" * width]
    out.append("PROTECTION POLICY".center(width))
    out.append("=" * width)
    for row in rows:
        out.append(row["branch"].ljust(20) +
                   " approvals=" + str(row["required_approvals"]) +
                   " checks=" + str(len(row["required_checks"])))
    return "\n".join(out)
