"""audit_encoder: encode AuditLog events."""
from __future__ import annotations


def encode_event(event):
    return {
        "at": event["at"], "actor": event["actor"],
        "kind": event["kind"], "target": event["target"],
        "payload": dict(event.get("payload") or {}),
    }


def encode_log(log):
    return [encode_event(e) for e in log.events()]


def decode_event(d):
    return {
        "at": d["at"], "actor": d["actor"], "kind": d["kind"],
        "target": d["target"], "payload": dict(d.get("payload") or {}),
    }
