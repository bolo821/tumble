#ifndef TUMBLE_NUMFMT_H
#define TUMBLE_NUMFMT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t tmb_fmt_float(float v, char *buf, size_t cap);
size_t tmb_fmt_int(long long v, char *buf, size_t cap);
int tmb_parse_float(const char *s, size_t n, float *out);
int tmb_parse_int(const char *s, size_t n, long long *out);
#ifdef __cplusplus
}
#endif
#endif
