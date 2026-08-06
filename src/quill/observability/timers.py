"""timers: time-a-window scoped counters."""
from __future__ import annotations


class TimerRegistry:
    def __init__(self):
        self._starts = {}
        self._durations = []

    def start(self, *, name, at):
        self._starts[name] = at
        return self

    def stop(self, *, name, at):
        start = self._starts.pop(name, None)
        if start is None:
            return 0
        duration = max(0, at - start)
        self._durations.append({"name": name, "duration": duration})
        return duration

    def total(self, name):
        return sum(d["duration"] for d in self._durations if d["name"] == name)

    def count(self, name):
        return sum(1 for d in self._durations if d["name"] == name)

    def average(self, name):
        c = self.count(name)
        if c == 0:
            return 0
        return self.total(name) // c
