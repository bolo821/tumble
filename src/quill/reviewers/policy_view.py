"""policy_view: view over CodeownerRegistry."""
from __future__ import annotations


def owners_matching(registry, paths):
    out = set()
    for path in paths:
        out.update(registry.owners_for(path))
    return sorted(out)


def path_coverage(registry, paths):
    covered = [p for p in paths if registry.owners_for(p)]
    if not paths:
        return 0.0
    return len(covered) / len(paths)


def uncovered_paths(registry, paths):
    return [p for p in paths if not registry.owners_for(p)]
