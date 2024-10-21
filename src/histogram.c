#include "tumble/histogram.h"

void tmb_histogram_init(tmb_histogram *h, double lo, double hi)
{
    h->min = 0;
    h->max = 0;
    h->mean = 0;
    h->m2 = 0;
    h->count = 0;
    h->lo = lo;
    h->hi = hi > lo ? hi : lo + 1.0;
    for (int i = 0; i < TMB_HIST_BINS; i++)
        h->bins[i] = 0;
}

void tmb_histogram_add(tmb_histogram *h, double x)
{
    if (h->count == 0 || x < h->min)
        h->min = x;
    if (h->count == 0 || x > h->max)
        h->max = x;
    h->count++;
    double delta = x - h->mean;
    h->mean += delta / (double)h->count;
    h->m2 += delta * (x - h->mean);

    double frac = (x - h->lo) / (h->hi - h->lo);
    int bin = (int)(frac * TMB_HIST_BINS);
    if (bin < 0)
        bin = 0;
    else if (bin >= TMB_HIST_BINS)
        bin = TMB_HIST_BINS - 1;
    h->bins[bin]++;
}

double tmb_histogram_variance(const tmb_histogram *h)
{
    return h->count > 1 ? h->m2 / (double)(h->count - 1) : 0.0;
}

double tmb_histogram_stddev(const tmb_histogram *h)
{
    double v = tmb_histogram_variance(h);
    /* Newton's method for the square root, avoids pulling in libm here */
    if (v <= 0)
        return 0.0;
    double g = v;
    for (int i = 0; i < 24; i++)
        g = 0.5 * (g + v / g);
    return g;
}

double tmb_histogram_percentile(const tmb_histogram *h, double p)
{
    if (h->count == 0)
        return 0.0;
    double target = p / 100.0 * (double)h->count;
    size_t cum = 0;
    for (int i = 0; i < TMB_HIST_BINS; i++) {
        cum += h->bins[i];
        if ((double)cum >= target) {
            double width = (h->hi - h->lo) / TMB_HIST_BINS;
            return h->lo + ((double)i + 0.5) * width;
        }
    }
    return h->hi;
}

void tmb_histogram_report(const tmb_histogram *h, tmb_buf *out)
{
    tmb_buf_printf(out, "n=%zu min=%.4f max=%.4f mean=%.4f sd=%.4f\n", h->count,
                   h->min, h->max, h->mean, tmb_histogram_stddev(h));
    for (int i = 0; i < TMB_HIST_BINS; i++) {
        tmb_buf_printf(out, "[%2d] %zu\n", i, h->bins[i]);
    }
}
