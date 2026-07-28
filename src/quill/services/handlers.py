"""handlers: pure callable handlers."""
from __future__ import annotations


def webhook_post(service, *, url):
    from quill.services.webhook import WebhookPost

    def handle(payload):
        service.record(WebhookPost(
            url=url, payload=dict(payload),
            at=payload.get("at", 0),
            status_code=payload.get("status_code", 200),
        ))
        return True
    return handle


def slack_message(service, *, channel):
    from quill.services.slack import SlackMessage

    def handle(payload):
        service.post(SlackMessage(
            channel=channel,
            text=payload.get("text", ""),
            at=payload.get("at", 0),
        ))
        return True
    return handle


def email_message(service, *, recipient):
    from quill.services.email import Email

    def handle(payload):
        service.send(Email(
            recipient=recipient,
            subject=payload.get("subject", ""),
            body=payload.get("body", ""),
            at=payload.get("at", 0),
        ))
        return True
    return handle
