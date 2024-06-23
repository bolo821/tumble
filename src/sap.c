#include "tumble/sap.h"

#include <stdlib.h>

void tmb_sap_init(tmb_sap *s)
{
    s->endpoints = NULL;
    s->ep_count = s->ep_cap = 0;
    s->pairs = NULL;
    s->pair_count = s->pair_cap = 0;
}

void tmb_sap_free(tmb_sap *s)
{
    free(s->endpoints);
    free(s->pairs);
    tmb_sap_init(s);
}

static int reserve_eps(tmb_sap *s, size_t n)
{
    if (n <= s->ep_cap)
        return 1;
    size_t cap = s->ep_cap ? s->ep_cap : 32;
    while (cap < n)
        cap *= 2;
    tmb_endpoint *e = (tmb_endpoint *)realloc(s->endpoints, cap * sizeof(*e));
    if (!e)
        return 0;
    s->endpoints = e;
    s->ep_cap = cap;
    return 1;
}

static int push_pair(tmb_sap *s, uint32_t a, uint32_t b)
{
    if (s->pair_count == s->pair_cap) {
        size_t cap = s->pair_cap ? s->pair_cap * 2 : 64;
        tmb_pair *p = (tmb_pair *)realloc(s->pairs, cap * sizeof(*p));
        if (!p)
            return 0;
        s->pairs = p;
        s->pair_cap = cap;
    }
    s->pairs[s->pair_count].a = a < b ? a : b;
    s->pairs[s->pair_count].b = a < b ? b : a;
    s->pair_count++;
    return 1;
}

/* insertion sort -- endpoints stay nearly sorted frame to frame, so this is
 * close to linear in practice */
static void sort_endpoints(tmb_endpoint *e, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        tmb_endpoint key = e[i];
        size_t j = i;
        while (j > 0 && e[j - 1].value > key.value) {
            e[j] = e[j - 1];
            j--;
        }
        e[j] = key;
    }
}

size_t tmb_sap_run(tmb_sap *s, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    if (!reserve_eps(s, n * 2))
        return (size_t)-1;
    s->ep_count = 0;
    s->pair_count = 0;

    /* cache y intervals while we build the x endpoint list */
    tmb_aabb *boxes = (tmb_aabb *)malloc((n ? n : 1) * sizeof(*boxes));
    if (!boxes)
        return (size_t)-1;

    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(w, (uint32_t)i);
        tmb_body_sync(b);
        tmb_aabb box = tmb_shape_aabb(&b->shape, b->xf);
        boxes[i] = box;
        s->endpoints[s->ep_count].value = box.min.x;
        s->endpoints[s->ep_count].body = (uint32_t)i;
        s->endpoints[s->ep_count].is_min = 1;
        s->ep_count++;
        s->endpoints[s->ep_count].value = box.max.x;
        s->endpoints[s->ep_count].body = (uint32_t)i;
        s->endpoints[s->ep_count].is_min = 0;
        s->ep_count++;
    }

    sort_endpoints(s->endpoints, s->ep_count);

    /* sweep: maintain the set of "open" intervals */
    uint32_t *active = (uint32_t *)malloc((n ? n : 1) * sizeof(*active));
    if (!active) {
        free(boxes);
        return (size_t)-1;
    }
    size_t nactive = 0;
    for (size_t i = 0; i < s->ep_count; i++) {
        tmb_endpoint *ep = &s->endpoints[i];
        if (ep->is_min) {
            for (size_t k = 0; k < nactive; k++) {
                uint32_t other = active[k];
                /* x already overlaps by construction; test y */
                if (boxes[ep->body].min.y <= boxes[other].max.y &&
                    boxes[ep->body].max.y >= boxes[other].min.y) {
                    if (!push_pair(s, ep->body, other)) {
                        free(boxes);
                        free(active);
                        return (size_t)-1;
                    }
                }
            }
            active[nactive++] = ep->body;
        } else {
            for (size_t k = 0; k < nactive; k++) {
                if (active[k] == ep->body) {
                    active[k] = active[--nactive];
                    break;
                }
            }
        }
    }

    free(boxes);
    free(active);
    return s->pair_count;
}
