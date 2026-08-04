"""exporter: export metrics to a text form."""
from __future__ import annotations


def counter_lines(counter, *, prefix="counter"):
    return [prefix + "." + k + " " + str(counter.get(k))
            for k in counter.keys()]


def gauge_lines(gauge, *, prefix="gauge"):
    return [prefix + "." + k + " " + str(gauge.get(k))
            for k in gauge.keys()]


def histogram_lines(histogram, *, prefix="hist"):
    return [
        prefix + ".count " + str(histogram.total_count()),
        prefix + ".avg " + str(histogram.average()),
    ]


def format_all(*, counters, gauges, histograms):
    out = []
    for name, c in counters.items():
        out.extend(counter_lines(c, prefix=name))
    for name, g in gauges.items():
        out.extend(gauge_lines(g, prefix=name))
    for name, h in histograms.items():
        out.extend(histogram_lines(h, prefix=name))
    return "\n".join(out)
