"""latency_report: merge-latency summary."""
from __future__ import annotations

from quill.analytics.latency import merge_latencies, average, median, percentile


def summary(reviews):
    lats = merge_latencies(reviews)
    return {
        "count": len(lats),
        "mean": average(lats),
        "median": median(lats),
        "p90": percentile(lats, p=90),
        "p99": percentile(lats, p=99),
    }


def render(reviews, *, width=60):
    out = ["=" * width]
    out.append("LATENCY REPORT".center(width))
    out.append("=" * width)
    s = summary(reviews)
    for k, v in s.items():
        out.append(k.ljust(15) + str(v))
    return "\n".join(out)
