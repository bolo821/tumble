#ifndef TUMBLE_ERRORS_H
#define TUMBLE_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Result codes across the Tumble API. TMB_OK is zero. */
typedef enum {
    TMB_OK = 0,
    TMB_ERR_IO,
    TMB_ERR_OOM,
    TMB_ERR_PARSE,
    TMB_ERR_BAD_MAGIC,
    TMB_ERR_BAD_VERSION,
    TMB_ERR_TRUNCATED,
    TMB_ERR_RANGE,       /* a count/size exceeded a sanity limit */
    TMB_ERR_INDEX,       /* a reference to a non-existent body/shape */
    TMB_ERR_DEGENERATE,  /* a shape with too few or collinear vertices */
    TMB_ERR_EMPTY,
    TMB_ERR_UNSUPPORTED,
    TMB_ERR__COUNT
} tmb_err;

const char *tmb_strerror(tmb_err err);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_ERRORS_H */
