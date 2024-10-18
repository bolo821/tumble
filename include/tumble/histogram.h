#ifndef TUMBLE_HISTOGRAM_H
#define TUMBLE_HISTOGRAM_H

#include "tumble/buffer.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A running summary of a stream of samples (min/max/mean/variance via
 * Welford) plus a fixed-bin histogram over a known range. Used to profile
 * contact counts, penetration depths and speeds across a long simulation. */

#define TMB_HIST_BINS 16

typedef struct {
    double min, max, mean, m2;
    size_t count;
    double lo, hi; /* histogram range */
    size_t bins[TMB_HIST_BINS];
} tmb_histogram;

void tmb_histogram_init(tmb_histogram *h, double lo, double hi);
void tmb_histogram_add(tmb_histogram *h, double x);
double tmb_histogram_variance(const tmb_histogram *h);
double tmb_histogram_stddev(const tmb_histogram *h);
/* Approximate percentile in [0,100] from the bins. */
double tmb_histogram_percentile(const tmb_histogram *h, double p);
void tmb_histogram_report(const tmb_histogram *h, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_HISTOGRAM_H */
