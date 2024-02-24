#include "tumble/buffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void tmb_buf_init(tmb_buf *b)
{
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
    b->oom = 0;
}

void tmb_buf_free(tmb_buf *b)
{
    free(b->data);
    tmb_buf_init(b);
}

int tmb_buf_reserve(tmb_buf *b, size_t extra)
{
    if (b->oom)
        return -1;
    if (extra <= b->cap - b->len)
        return 0;
    size_t need = b->len + extra;
    if (need < b->len) {
        b->oom = 1;
        return -1;
    }
    size_t cap = b->cap ? b->cap : 64;
    while (cap < need) {
        size_t d = cap * 2;
        if (d < cap) {
            cap = need;
            break;
        }
        cap = d;
    }
    uint8_t *p = (uint8_t *)realloc(b->data, cap);
    if (!p) {
        b->oom = 1;
        return -1;
    }
    b->data = p;
    b->cap = cap;
    return 0;
}

int tmb_buf_put(tmb_buf *b, uint8_t byte)
{
    if (tmb_buf_reserve(b, 1) != 0)
        return -1;
    b->data[b->len++] = byte;
    return 0;
}

int tmb_buf_write(tmb_buf *b, const void *src, size_t n)
{
    if (n == 0)
        return 0;
    if (tmb_buf_reserve(b, n) != 0)
        return -1;
    memcpy(b->data + b->len, src, n);
    b->len += n;
    return 0;
}

int tmb_buf_puts(tmb_buf *b, const char *s) { return tmb_buf_write(b, s, strlen(s)); }

int tmb_buf_printf(tmb_buf *b, const char *fmt, ...)
{
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (n < 0) { va_end(ap2); return -1; }
    if (tmb_buf_reserve(b, (size_t)n + 1) != 0) { va_end(ap2); return -1; }
    vsnprintf((char *)b->data + b->len, (size_t)n + 1, fmt, ap2);
    va_end(ap2);
    b->len += (size_t)n;
    return 0;
}
