#ifndef TUMBLE_BUFFER_H
#define TUMBLE_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A growable byte buffer used by the scene writers and the ASCII renderer. */
typedef struct {
    uint8_t *data;
    size_t len;
    size_t cap;
    int oom;
} tmb_buf;

void tmb_buf_init(tmb_buf *b);
void tmb_buf_free(tmb_buf *b);
int tmb_buf_reserve(tmb_buf *b, size_t extra);
int tmb_buf_put(tmb_buf *b, uint8_t byte);
int tmb_buf_write(tmb_buf *b, const void *src, size_t n);
int tmb_buf_puts(tmb_buf *b, const char *s);
int tmb_buf_printf(tmb_buf *b, const char *fmt, ...);

/* Drop the current contents but keep the allocation for reuse. Also clears a
 * sticky OOM flag so a buffer can be reused after a failed grow. */
void tmb_buf_reset(tmb_buf *b);
/* Ensure a trailing NUL and return the contents as a C string, or NULL if the
 * terminator could not be reserved. The NUL is not counted in `len`. */
char *tmb_buf_cstr(tmb_buf *b);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BUFFER_H */
