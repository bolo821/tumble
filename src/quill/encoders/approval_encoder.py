"""approval_encoder: encode Approvals."""
from __future__ import annotations

from quill.approvals.approval import Approval


def encode(a):
    return {
        "review_id": a.review_id, "reviewer": a.reviewer,
        "decision": a.decision, "at": a.at, "body": a.body,
    }


def decode(d):
    return Approval(
        review_id=d["review_id"], reviewer=d["reviewer"],
        decision=d["decision"], at=d["at"], body=d.get("body", ""),
    )


def encode_many(items):
    return [encode(a) for a in items]


def decode_many(records):
    return [decode(r) for r in records]
