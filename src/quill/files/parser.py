"""parser: parse a diff string into Hunks."""
from __future__ import annotations

import re

from quill.files.hunk import Hunk


HUNK_RE = re.compile(r"^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@")


def parse_hunks(diff_text, *, file_path):
    out = []
    body_lines = []
    header = None
    for line in diff_text.splitlines():
        m = HUNK_RE.match(line)
        if m:
            if header is not None:
                out.append(_finalize(header, body_lines, file_path))
                body_lines = []
            header = m
        elif header is not None:
            body_lines.append(line)
    if header is not None:
        out.append(_finalize(header, body_lines, file_path))
    return out


def _finalize(header, body_lines, file_path):
    old_start = int(header.group(1))
    old_lines = int(header.group(2) or 1)
    new_start = int(header.group(3))
    new_lines = int(header.group(4) or 1)
    return Hunk(
        file_path=file_path,
        old_start=old_start, old_lines=old_lines,
        new_start=new_start, new_lines=new_lines,
        body="\n".join(body_lines),
    )
