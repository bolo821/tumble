#ifndef TTEST_H
#define TTEST_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int tt_failures = 0;
static int tt_checks = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        tt_checks++;                                                           \
        if (!(cond)) {                                                         \
            tt_failures++;                                                     \
            fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);  \
        }                                                                      \
    } while (0)

#define CHECK_NEAR(a, b, eps)                                                  \
    do {                                                                       \
        tt_checks++;                                                           \
        if (fabs((double)(a) - (double)(b)) > (eps)) {                         \
            tt_failures++;                                                     \
            fprintf(stderr, "  FAIL %s:%d: |%f - %f| > %f\n", __FILE__,        \
                    __LINE__, (double)(a), (double)(b), (double)(eps));        \
        }                                                                      \
    } while (0)

#define TT_DONE()                                                              \
    do {                                                                       \
        fprintf(stderr, "%s: %d checks, %d failures\n", __FILE__, tt_checks,   \
                tt_failures);                                                  \
        return tt_failures ? 1 : 0;                                            \
    } while (0)

#endif
