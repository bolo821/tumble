#ifndef TUMBLE_SCENE_PARSE_H
#define TUMBLE_SCENE_PARSE_H

#include <stddef.h>

#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bounded line reader and tokenizer for the text scene format.
 *
 * The text loader assembles one *logical* line at a time, following trailing
 * backslash continuations, then splits it into whitespace-separated tokens.
 * Both steps are bounded so that a hostile or malformed input can never drive
 * an out-of-bounds write or unbounded memory growth:
 *
 *   - a logical line may not exceed TMB_SCENE_MAX_LINE bytes (cumulative,
 *     across every continued segment),
 *   - at most TMB_SCENE_MAX_CONT physical lines may be joined into one
 *     logical line, and
 *   - a logical line splits into at most TMB_SCENE_MAX_TOKENS tokens.
 *
 * The assembled line lives in a caller-owned growable tmb_buf, so there is no
 * fixed-size stack or heap buffer to overrun regardless of the input. */

#define TMB_SCENE_MAX_LINE   65536u
#define TMB_SCENE_MAX_CONT    4096u
#define TMB_SCENE_MAX_TOKENS  4096u

typedef enum {
    TMB_LINE_OK = 0,   /* a logical line was assembled into `out` */
    TMB_LINE_END,      /* no more input */
    TMB_LINE_OVERFLOW  /* a cap was hit; the offending run was consumed+skipped */
} tmb_line_status;

/* Cursor over an in-memory text blob. */
typedef struct {
    const char *text;
    size_t len;
    size_t pos;
} tmb_line_reader;

void tmb_line_reader_init(tmb_line_reader *r, const char *text, size_t len);

/* Assemble the next logical line into `out` (which is reset first). On
 * TMB_LINE_OK, `out` holds the NUL-terminated line and tmb_buf_cstr(out) is a
 * valid C string. On TMB_LINE_OVERFLOW the offending physical-line run has
 * been consumed and the caller should simply continue. TMB_LINE_END signals
 * end of input. */
tmb_line_status tmb_line_reader_next(tmb_line_reader *r, tmb_buf *out);

/* In-place whitespace tokenizer. `tok` entries point into the mutable line
 * buffer; separators are overwritten with '\0'. The token array grows as
 * needed and is owned by the tmb_tokens object. */
typedef struct {
    char **tok;
    size_t n;
    size_t cap;
} tmb_tokens;

void tmb_tokens_init(tmb_tokens *t);
void tmb_tokens_free(tmb_tokens *t);

/* Split `line` in place into `t` (reset first). Returns 0 on success or -1 on
 * allocation failure. Stops after TMB_SCENE_MAX_TOKENS tokens. */
int tmb_tokens_split(tmb_tokens *t, char *line);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SCENE_PARSE_H */
