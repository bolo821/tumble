#include "tumble/profile.h"

#include <string.h>

void tmb_profile_init(tmb_profile *p)
{
    memset(p, 0, sizeof(*p));
}

int tmb_profile_slot(tmb_profile *p, const char *name)
{
    for (int i = 0; i < p->used; i++)
        if (p->names[i] && strcmp(p->names[i], name) == 0)
            return i;
    if (p->used >= TMB_PROFILE_SLOTS)
        return -1;
    int idx = p->used++;
    p->names[idx] = name;
    p->accum[idx] = 0;
    p->counts[idx] = 0;
    return idx;
}

void tmb_profile_add(tmb_profile *p, const char *name, double value)
{
    int i = tmb_profile_slot(p, name);
    if (i < 0)
        return;
    p->accum[i] += value;
    p->counts[i]++;
}

double tmb_profile_total(const tmb_profile *p, const char *name)
{
    for (int i = 0; i < p->used; i++)
        if (p->names[i] && strcmp(p->names[i], name) == 0)
            return p->accum[i];
    return 0.0;
}

void tmb_profile_reset(tmb_profile *p)
{
    for (int i = 0; i < p->used; i++) {
        p->accum[i] = 0;
        p->counts[i] = 0;
    }
}

void tmb_profile_report(const tmb_profile *p, tmb_buf *out)
{
    for (int i = 0; i < p->used; i++) {
        double mean = p->counts[i] ? p->accum[i] / (double)p->counts[i] : 0.0;
        tmb_buf_printf(out, "%-16s total=%.4f n=%zu mean=%.4f\n",
                       p->names[i] ? p->names[i] : "?", p->accum[i],
                       p->counts[i], mean);
    }
}
