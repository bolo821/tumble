"""scope: resolve permissions across scope chain."""
from __future__ import annotations


def resolve(*, acl, scopes, principal_id, permission, now=0):
    for scope in scopes:
        for entry in acl.for_target(scope):
            if entry.is_expired(now=now):
                continue
            if entry.principal_id != principal_id:
                continue
            if entry.permission == permission or entry.permission == "admin":
                return scope, True
    return None, False


def is_authorized(*, acl, scopes, principal_id, permission, now=0):
    return resolve(acl=acl, scopes=scopes, principal_id=principal_id,
                   permission=permission, now=now)[1]


def authorized_scopes(*, acl, scopes, principal_id, permission, now=0):
    out = []
    for scope in scopes:
        for entry in acl.for_target(scope):
            if entry.is_expired(now=now):
                continue
            if entry.principal_id != principal_id:
                continue
            if entry.permission == permission or entry.permission == "admin":
                out.append(scope)
                break
    return out
