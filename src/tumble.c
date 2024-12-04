#include "tumble/tumble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *tmb_version(void) { return TMB_VERSION_STRING; }

tmb_err tmb_load_file(const char *path, tmb_world *out)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return TMB_ERR_IO;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return TMB_ERR_IO; }
    rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz ? (size_t)sz : 1);
    if (!buf) { fclose(f); return TMB_ERR_OOM; }
    size_t got = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    tmb_err e;
    if (got >= 4 && memcmp(buf, "TMBB", 4) == 0)
        e = tmb_load_scene_bin(buf, got, out);
    else
        e = tmb_load_scene_text((const char *)buf, got, out);
    free(buf);
    return e;
}
