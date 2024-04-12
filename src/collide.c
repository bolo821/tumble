#include "tumble/collide.h"

#include <math.h>

/* World-space polygon: vertices and outward edge normals transformed by xf. */
typedef struct {
    tmb_vec2 v[TMB_MAX_POLY_VERTS];
    tmb_vec2 n[TMB_MAX_POLY_VERTS];
    int count;
} world_poly;

static void to_world(const tmb_shape *s, tmb_xform xf, world_poly *wp)
{
    wp->count = s->count;
    for (int i = 0; i < s->count; i++) {
        wp->v[i] = tmb_xform_point(xf, s->verts[i]);
        wp->n[i] = tmb_xform_dir(xf, s->normals[i]);
    }
}

static void emit_circle_circle(tmb_world *w, uint32_t ai, uint32_t bi,
                               tmb_body *A, tmb_body *B)
{
    tmb_vec2 d = tmb_v2_sub(B->pos, A->pos);
    float r = A->shape.radius + B->shape.radius;
    float dist = tmb_v2_len(d);
    if (dist >= r)
        return;
    tmb_contact *c = tmb_contact_alloc(w);
    if (!c)
        return;
    tmb_vec2 n = dist > 1e-6f ? tmb_v2_scale(d, 1.0f / dist) : tmb_v2(0, 1);
    c->a = ai;
    c->b = bi;
    c->normal = n;
    c->point = tmb_v2_add(A->pos, tmb_v2_scale(n, A->shape.radius));
    c->penetration = r - dist;
    c->restitution = tmb_maxf(A->restitution, B->restitution);
    c->friction = sqrtf(A->friction * B->friction);
}

static void emit_circle_poly(tmb_world *w, uint32_t ci, uint32_t pi,
                             tmb_body *C, tmb_body *P, int circle_is_a)
{
    /* circle center in the polygon's local frame */
    tmb_vec2 local = tmb_xform_unpoint(P->xf, C->pos);
    const tmb_shape *s = &P->shape;
    float sep = -1e30f;
    int face = 0;
    for (int i = 0; i < s->count; i++) {
        float d = tmb_v2_dot(s->normals[i], tmb_v2_sub(local, s->verts[i]));
        if (d > sep) {
            sep = d;
            face = i;
        }
    }
    if (sep > C->shape.radius)
        return;

    tmb_vec2 va = s->verts[face];
    tmb_vec2 vb = s->verts[(face + 1) % s->count];
    tmb_vec2 normal_local;
    tmb_vec2 point_local;
    if (sep < 1e-6f) {
        normal_local = s->normals[face];
        point_local = tmb_v2_scale(tmb_v2_add(va, vb), 0.5f);
    } else {
        /* nearest edge region (Voronoi): clamp to the edge */
        float u1 = tmb_v2_dot(tmb_v2_sub(local, va), tmb_v2_sub(vb, va));
        float u2 = tmb_v2_dot(tmb_v2_sub(local, vb), tmb_v2_sub(va, vb));
        if (u1 <= 0.0f) {
            point_local = va;
        } else if (u2 <= 0.0f) {
            point_local = vb;
        } else {
            point_local = tmb_v2_lerp(va, vb, u1 / (u1 + u2));
        }
        normal_local = tmb_v2_normalize(tmb_v2_sub(local, point_local));
        if (tmb_v2_len_sq(tmb_v2_sub(local, point_local)) >
            C->shape.radius * C->shape.radius)
            return;
    }

    tmb_contact *c = tmb_contact_alloc(w);
    if (!c)
        return;
    tmb_vec2 n_world = tmb_xform_dir(P->xf, normal_local);
    /* normal must point from a to b */
    if (circle_is_a)
        n_world = tmb_v2_neg(n_world);
    c->a = circle_is_a ? ci : pi;
    c->b = circle_is_a ? pi : ci;
    c->normal = n_world;
    c->point = tmb_xform_point(P->xf, point_local);
    c->penetration = C->shape.radius - sep;
    c->restitution = tmb_maxf(C->restitution, P->restitution);
    c->friction = sqrtf(C->friction * P->friction);
}

/* Largest separation of B's vertices from A's faces. */
static float max_separation(const world_poly *A, const world_poly *B, int *face)
{
    float best = -1e30f;
    int bestFace = 0;
    for (int i = 0; i < A->count; i++) {
        tmb_vec2 n = A->n[i];
        float minDot = 1e30f;
        for (int j = 0; j < B->count; j++) {
            float d = tmb_v2_dot(n, tmb_v2_sub(B->v[j], A->v[i]));
            if (d < minDot)
                minDot = d;
        }
        if (minDot > best) {
            best = minDot;
            bestFace = i;
        }
    }
    *face = bestFace;
    return best;
}

/* Incident edge of B: the edge most anti-parallel to the reference normal. */
static int incident_edge(const world_poly *B, tmb_vec2 ref_normal)
{
    int idx = 0;
    float minDot = 1e30f;
    for (int i = 0; i < B->count; i++) {
        float d = tmb_v2_dot(ref_normal, B->n[i]);
        if (d < minDot) {
            minDot = d;
            idx = i;
        }
    }
    return idx;
}

typedef struct {
    tmb_vec2 point;
    float depth;
} clip_pt;

static void emit_poly_poly(tmb_world *w, uint32_t ai, uint32_t bi, tmb_body *A,
                           tmb_body *B)
{
    world_poly WA, WB;
    to_world(&A->shape, A->xf, &WA);
    to_world(&B->shape, B->xf, &WB);

    int faceA, faceB;
    float sepA = max_separation(&WA, &WB, &faceA);
    if (sepA >= 0.0f)
        return; /* separating axis on A */
    float sepB = max_separation(&WB, &WA, &faceB);
    if (sepB >= 0.0f)
        return;

    /* reference polygon = the one with the larger (less negative) separation */
    const world_poly *ref;
    const world_poly *inc;
    int refFace;
    uint32_t refBody, incBody;
    int flip;
    if (sepA >= sepB) {
        ref = &WA; inc = &WB; refFace = faceA; refBody = ai; incBody = bi; flip = 0;
    } else {
        ref = &WB; inc = &WA; refFace = faceB; refBody = bi; incBody = ai; flip = 1;
    }

    tmb_vec2 refNormal = ref->n[refFace];
    int ie = incident_edge(inc, refNormal);
    tmb_vec2 i0 = inc->v[ie];
    tmb_vec2 i1 = inc->v[(ie + 1) % inc->count];

    /* keep incident points that are below the reference face plane */
    tmb_vec2 refV = ref->v[refFace];
    clip_pt cps[2];
    int ncp = 0;
    tmb_vec2 pts[2] = {i0, i1};
    for (int i = 0; i < 2; i++) {
        float depth = tmb_v2_dot(refNormal, tmb_v2_sub(pts[i], refV));
        if (depth <= 0.0f && ncp < 2) {
            cps[ncp].point = pts[i];
            cps[ncp].depth = -depth;
            ncp++;
        }
    }
    if (ncp == 0)
        return;

    tmb_body *bodyA = &w->bodies[ai];
    tmb_body *bodyB = &w->bodies[bi];
    float rest = tmb_maxf(bodyA->restitution, bodyB->restitution);
    float fric = sqrtf(bodyA->friction * bodyB->friction);
    /* contact normal must point from body a to body b */
    tmb_vec2 n = flip ? tmb_v2_neg(refNormal) : refNormal;

    /* Append a contact per clip point. For a two-point manifold we link the two
     * contacts so the solver can pair them for warm-starting. */
    tmb_contact *first = NULL;
    for (int i = 0; i < ncp; i++) {
        tmb_contact *c = tmb_contact_alloc(w);
        if (!c)
            return;
        c->a = ai;
        c->b = bi;
        c->normal = n;
        c->point = cps[i].point;
        c->penetration = cps[i].depth;
        c->restitution = rest;
        c->friction = fric;
        if (first == NULL) {
            first = c;
        } else {
            /* pair the two points of this manifold */
            first->sibling = (uint32_t)(c - w->contacts);
            c->sibling = (uint32_t)(first - w->contacts);
        }
    }
    (void)refBody;
    (void)incBody;
}

int tmb_collide(tmb_world *w, uint32_t ai, uint32_t bi)
{
    tmb_body *A = &w->bodies[ai];
    tmb_body *B = &w->bodies[bi];
    if (A->type != TMB_BODY_DYNAMIC && B->type != TMB_BODY_DYNAMIC)
        return 0; /* two static bodies never collide */

    size_t before = w->contact_count;
    int ka = A->shape.kind, kb = B->shape.kind;
    if (ka == TMB_SHAPE_CIRCLE && kb == TMB_SHAPE_CIRCLE)
        emit_circle_circle(w, ai, bi, A, B);
    else if (ka == TMB_SHAPE_CIRCLE && kb == TMB_SHAPE_POLYGON)
        emit_circle_poly(w, ai, bi, A, B, 1);
    else if (ka == TMB_SHAPE_POLYGON && kb == TMB_SHAPE_CIRCLE)
        emit_circle_poly(w, bi, ai, B, A, 0);
    else
        emit_poly_poly(w, ai, bi, A, B);
    return (int)(w->contact_count - before);
}
