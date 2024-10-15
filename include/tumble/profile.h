#ifndef TUMBLE_PROFILE_H
#define TUMBLE_PROFILE_H

#include "tumble/buffer.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Lightweight per-phase counters for the step pipeline. The engine bumps these
 * (broadphase pairs tested, contacts found, solver iterations, etc.) so a tool
 * can show where time and work go without any platform timer dependency. */

#define TMB_PROFILE_SLOTS 8

typedef struct {
    const char *names[TMB_PROFILE_SLOTS];
    double accum[TMB_PROFILE_SLOTS];
    size_t counts[TMB_PROFILE_SLOTS];
    int used;
} tmb_profile;

void tmb_profile_init(tmb_profile *p);
/* Find or create a slot by name; returns its index or -1 if full. */
int tmb_profile_slot(tmb_profile *p, const char *name);
void tmb_profile_add(tmb_profile *p, const char *name, double value);
double tmb_profile_total(const tmb_profile *p, const char *name);
void tmb_profile_reset(tmb_profile *p);
void tmb_profile_report(const tmb_profile *p, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_PROFILE_H */
