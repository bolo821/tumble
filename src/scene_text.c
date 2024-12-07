#include "tumble/tumble.h"

#include <stdlib.h>
#include <string.h>

/* Parser for the Tumble text scene format. One directive per line:
 *
 *   gravity GX GY
 *   iters VEL POS
 *   circle X Y R [static|dynamic] [density]
 *   box X Y HW HH [ANGLE] [static|dynamic] [density]
 *   poly X Y N x0 y0 ... [static|dynamic]
 *   joint distance A B AX AY BX BY REST
 *   joint revolute A B AX AY BX BY
 *   # comment
 *
 * Defensive: every field is checked, unknown directives are skipped, counts
 * are capped. */

#define MAXTOK 80

typedef struct {
    char *tok[MAXTOK];
    int n;
} line_toks;

static int is_sep(char c) { return c == ' ' || c == '\t' || c == '\r'; }

static void tokenize(char *line, line_toks *lt)
{
    lt->n = 0;
    char *p = line;
    while (*p && lt->n < MAXTOK) {
        while (*p && is_sep(*p))
            p++;
        if (!*p)
            break;
        lt->tok[lt->n++] = p;
        while (*p && !is_sep(*p))
            p++;
        if (*p)
            *p++ = '\0';
    }
}

static int body_type_of(const char *s) /* default dynamic */
{
    if (s && strcmp(s, "static") == 0)
        return TMB_BODY_STATIC;
    return TMB_BODY_DYNAMIC;
}

static tmb_err add_with(tmb_world *w, tmb_shape shape, int type, float x,
                        float y, float angle, float density)
{
    if (tmb_world_body_count(w) >= TMB_SCENE_MAX_BODIES)
        return TMB_ERR_RANGE;
    tmb_body b;
    tmb_body_init(&b, (tmb_body_type)type, shape, tmb_v2(x, y));
    b.angle = angle;
    if (type == TMB_BODY_DYNAMIC && density > 0.0f)
        tmb_body_set_density(&b, density);
    return tmb_world_add_body(w, &b) == UINT32_MAX ? TMB_ERR_OOM : TMB_OK;
}

static tmb_err handle(tmb_world *w, line_toks *lt)
{
    if (lt->n == 0 || lt->tok[0][0] == '#')
        return TMB_OK;
    const char *kw = lt->tok[0];

    if (strcmp(kw, "gravity") == 0 && lt->n >= 3) {
        w->gravity = tmb_v2((float)atof(lt->tok[1]), (float)atof(lt->tok[2]));
        return TMB_OK;
    }
    if (strcmp(kw, "iters") == 0 && lt->n >= 3) {
        w->velocity_iters = atoi(lt->tok[1]);
        w->position_iters = atoi(lt->tok[2]);
        if (w->velocity_iters < 1) w->velocity_iters = 1;
        if (w->velocity_iters > 100) w->velocity_iters = 100;
        if (w->position_iters < 0) w->position_iters = 0;
        if (w->position_iters > 100) w->position_iters = 100;
        return TMB_OK;
    }
    if (strcmp(kw, "circle") == 0 && lt->n >= 4) {
        float r = (float)atof(lt->tok[3]);
        if (r <= 0.0f || r > 1e6f)
            return TMB_OK;
        int type = lt->n >= 5 ? body_type_of(lt->tok[4]) : TMB_BODY_DYNAMIC;
        float dens = lt->n >= 6 ? (float)atof(lt->tok[5]) : 1.0f;
        return add_with(w, tmb_shape_circle(r), type,
                        (float)atof(lt->tok[1]), (float)atof(lt->tok[2]), 0.0f,
                        dens);
    }
    if (strcmp(kw, "box") == 0 && lt->n >= 5) {
        float hw = (float)atof(lt->tok[3]), hh = (float)atof(lt->tok[4]);
        if (hw <= 0.0f || hh <= 0.0f || hw > 1e6f || hh > 1e6f)
            return TMB_OK;
        float angle = lt->n >= 6 ? (float)atof(lt->tok[5]) : 0.0f;
        int type = lt->n >= 7 ? body_type_of(lt->tok[6]) : TMB_BODY_DYNAMIC;
        float dens = lt->n >= 8 ? (float)atof(lt->tok[7]) : 1.0f;
        return add_with(w, tmb_shape_box(hw, hh), type, (float)atof(lt->tok[1]),
                        (float)atof(lt->tok[2]), angle, dens);
    }
    if (strcmp(kw, "poly") == 0 && lt->n >= 4) {
        int count = atoi(lt->tok[3]);
        if (count < 3 || count > TMB_MAX_POLY_VERTS)
            return TMB_OK;
        if (lt->n < 4 + 2 * count)
            return TMB_ERR_PARSE;
        tmb_vec2 v[TMB_MAX_POLY_VERTS];
        for (int i = 0; i < count; i++)
            v[i] = tmb_v2((float)atof(lt->tok[4 + 2 * i]),
                          (float)atof(lt->tok[5 + 2 * i]));
        tmb_shape s;
        if (tmb_shape_polygon(&s, v, count) != TMB_OK)
            return TMB_OK; /* skip a degenerate polygon */
        int type = (lt->n > 4 + 2 * count)
                       ? body_type_of(lt->tok[4 + 2 * count])
                       : TMB_BODY_DYNAMIC;
        return add_with(w, s, type, (float)atof(lt->tok[1]),
                        (float)atof(lt->tok[2]), 0.0f, 1.0f);
    }
    if (strcmp(kw, "joint") == 0 && lt->n >= 9) {
        tmb_joint j;
        memset(&j, 0, sizeof(j));
        j.kind = strcmp(lt->tok[1], "revolute") == 0 ? TMB_JOINT_REVOLUTE
                                                      : TMB_JOINT_DISTANCE;
        j.a = (uint32_t)strtoul(lt->tok[2], NULL, 10);
        j.b = (uint32_t)strtoul(lt->tok[3], NULL, 10);
        j.anchor_a = tmb_v2((float)atof(lt->tok[4]), (float)atof(lt->tok[5]));
        j.anchor_b = tmb_v2((float)atof(lt->tok[6]), (float)atof(lt->tok[7]));
        j.rest_length = (float)atof(lt->tok[8]);
        j.stiffness = 1.0f;
        tmb_world_add_joint(w, &j); /* ignores out-of-range body refs */
        return TMB_OK;
    }
    return TMB_OK; /* unknown directive */
}

tmb_err tmb_load_scene_text(const char *text, size_t len, tmb_world *out)
{
    char buf[4096];
    size_t pos = 0;
    while (pos < len) {
        size_t eol = pos;
        while (eol < len && text[eol] != '\n')
            eol++;
        size_t n = eol - pos;
        if (n < sizeof(buf)) {
            memcpy(buf, text + pos, n);
            buf[n] = '\0';
            line_toks lt;
            tokenize(buf, &lt);
            tmb_err e = handle(out, &lt);
            if (e == TMB_ERR_OOM)
                return e;
        }
        pos = eol + 1;
    }
    return TMB_OK;
}
