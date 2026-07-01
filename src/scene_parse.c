#include "tumble/scene_parse.h"

#include <stdlib.h>
#include <string.h>

static int is_sep(char c) { return c == ' ' || c == '\t' || c == '\r'; }

void tmb_line_reader_init(tmb_line_reader *r, const char *text, size_t len)
{
    r->text = text;
    r->len = len;
    r->pos = 0;
}

/* Scan one physical line starting at r->pos. Advances r->pos past the newline
 * (if any). Returns the segment [*start, *start + *n) with any trailing CR
 * trimmed, and sets *continued if the segment ended in a backslash (which is
 * excluded from *n). */
static void scan_physical(tmb_line_reader *r, size_t *start, size_t *n,
                          int *continued)
{
    size_t s = r->pos;
    size_t eol = r->pos;
    while (eol < r->len && r->text[eol] != '\n')
        eol++;
    size_t len = eol - s;
    r->pos = (eol < r->len) ? eol + 1 : eol;

    if (len > 0 && r->text[s + len - 1] == '\r')
        len--;

    int cont = (len > 0 && r->text[s + len - 1] == '\\');
    if (cont)
        len--;

    *start = s;
    *n = len;
    *continued = cont;
}

tmb_line_status tmb_line_reader_next(tmb_line_reader *r, tmb_buf *out)
{
    if (r->pos >= r->len)
        return TMB_LINE_END;

    tmb_buf_reset(out);

    int continued;
    int overflow = 0;
    size_t joined = 0;

    do {
        size_t start, n;
        scan_physical(r, &start, &n, &continued);

        /* Enforce the cumulative cap *before* copying so a run of continued
         * segments can never grow the logical line without bound. This is the
         * check the fixed-buffer implementation was missing: it validated each
         * segment in isolation while appending at an ever-growing offset. */
        if (!overflow) {
            if (out->len + n > TMB_SCENE_MAX_LINE)
                overflow = 1;
            else if (tmb_buf_write(out, r->text + start, n) != 0)
                overflow = 1; /* allocation failure: treat as skip */
        }

        if (++joined > TMB_SCENE_MAX_CONT)
            overflow = 1;

        /* Keep consuming the physical lines of a continued run even after an
         * overflow so the caller resumes cleanly on the next logical line. */
    } while (continued && r->pos < r->len);

    if (overflow)
        return TMB_LINE_OVERFLOW;

    if (!tmb_buf_cstr(out))
        return TMB_LINE_OVERFLOW;
    return TMB_LINE_OK;
}

void tmb_tokens_init(tmb_tokens *t)
{
    t->tok = NULL;
    t->n = 0;
    t->cap = 0;
}

void tmb_tokens_free(tmb_tokens *t)
{
    free(t->tok);
    tmb_tokens_init(t);
}

static int tokens_push(tmb_tokens *t, char *s)
{
    if (t->n == t->cap) {
        size_t cap = t->cap ? t->cap * 2 : 16;
        char **nt = (char **)realloc(t->tok, cap * sizeof(*nt));
        if (!nt)
            return -1;
        t->tok = nt;
        t->cap = cap;
    }
    t->tok[t->n++] = s;
    return 0;
}

int tmb_tokens_split(tmb_tokens *t, char *line)
{
    t->n = 0;
    char *p = line;
    while (*p && t->n < TMB_SCENE_MAX_TOKENS) {
        while (*p && is_sep(*p))
            p++;
        if (!*p)
            break;
        char *start = p;
        while (*p && !is_sep(*p))
            p++;
        if (*p)
            *p++ = '\0';
        if (tokens_push(t, start) != 0)
            return -1;
    }
    return 0;
}
