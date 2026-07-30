"""RetryPolicy: declarative retry rules."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class RetryPolicy:
    max_attempts: int = 1
    retry_on: tuple = ()
    no_retry_on: tuple = ()

    def is_retryable(self, *, attempt, signal):
        if attempt >= self.max_attempts:
            return False
        if signal in self.no_retry_on:
            return False
        if not self.retry_on:
            return True
        return signal in self.retry_on


def retries_remaining(*, policy, attempt):
    return max(0, policy.max_attempts - attempt)


def should_retry(*, policy, attempt, signal):
    return policy.is_retryable(attempt=attempt, signal=signal)
