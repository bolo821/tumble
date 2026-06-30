"""CLI entry point. quill is primarily a library."""
from __future__ import annotations

import sys


def main(argv=None):
    argv = argv or sys.argv[1:]
    if not argv or argv[0] in ("-h", "--help"):
        print("quill: code-review platform library; no shell commands yet.")
        return 0
    print("quill: unknown command:", argv[0])
    return 2


if __name__ == "__main__":
    sys.exit(main())
