"""MetricsRegistry: bundle counters / gauges / histograms."""
from __future__ import annotations


class MetricsRegistry:
    def __init__(self):
        self._counters = {}
        self._gauges = {}
        self._histograms = {}

    def register_counter(self, name, counter):
        self._counters[name] = counter
        return self

    def register_gauge(self, name, gauge):
        self._gauges[name] = gauge
        return self

    def register_histogram(self, name, histogram):
        self._histograms[name] = histogram
        return self

    def counter(self, name):
        return self._counters.get(name)

    def gauge(self, name):
        return self._gauges.get(name)

    def histogram(self, name):
        return self._histograms.get(name)

    def counter_names(self):
        return sorted(self._counters)

    def gauge_names(self):
        return sorted(self._gauges)

    def histogram_names(self):
        return sorted(self._histograms)
