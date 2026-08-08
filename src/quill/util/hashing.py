"""hashing: pure integer-hash helpers."""
from __future__ import annotations

import hashlib


def digest_hex(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def short_digest(text, *, length=8):
    return digest_hex(text)[:length]


def combined_digest(parts):
    joined = "\x00".join(str(p) for p in parts)
    return digest_hex(joined)
