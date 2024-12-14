/* tumble — command line front end.
 *
 * Subcommands:
 *   version                         print the library version
 *   run     <scene> [steps] [dt]    simulate and print final body states
 *   render  <scene> [w] [h] [steps] simulate and draw an ASCII snapshot
 *   stats   <scene>                 print a summary of a scene
 *   convert <in> <out>              load a scene and re-serialize it
 *   gen     <kind> <out> [seed]     procedurally generate a scene
 *   bench   <scene> [steps]         run many steps and report throughput
 *
 * The CLI is deliberately excluded from the fuzzing build; the fuzz targets
 * call the library entry points directly. */

#include "tumble/tumble.h"
#include "tumble/stats.h"
#include "tumble/ascii_view.h"
#include "tumble/builder.h"
#include "tumble/scene_gen.h"
#include "tumble/rng.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int write_file(const char *path, const uint8_t *data, size_t len)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    size_t n = fwrite(data, 1, len, f);
    fclose(f);
    return n == len ? 0 : -1;
}

static int load_world(const char *path, tmb_world *w)
{
    tmb_err e = tmb_load_file(path, w);
    if (e != TMB_OK) {
        fprintf(stderr, "load '%s' failed: %s\n", path, tmb_strerror(e));
        return -1;
    }
    return 0;
}

static int cmd_run(int argc, char **argv)
{
    if (argc < 1) {
        fprintf(stderr, "usage: tumble run <scene> [steps] [dt]\n");
        return 2;
    }
    int steps = argc >= 2 ? atoi(argv[1]) : 60;
    float dt = argc >= 3 ? (float)atof(argv[2]) : 1.0f / 60.0f;
    if (steps < 0)
        steps = 0;
    tmb_world w;
    if (load_world(argv[0], &w) != 0)
        return 1;
    for (int i = 0; i < steps; i++)
        tmb_world_step(&w, dt);
    size_t n = tmb_world_body_count(&w);
    printf("# %zu bodies after %d steps (dt=%.5f)\n", n, steps, dt);
    for (size_t i = 0; i < n; i++) {
        tmb_body *b = tmb_world_body(&w, (uint32_t)i);
        printf("body %3zu  pos=(% .4f,% .4f)  angle=% .4f  vel=(% .4f,% .4f)\n",
               i, b->pos.x, b->pos.y, b->angle, b->vel.x, b->vel.y);
    }
    tmb_world_free(&w);
    return 0;
}

static int cmd_render(int argc, char **argv)
{
    if (argc < 1) {
        fprintf(stderr, "usage: tumble render <scene> [w] [h] [steps]\n");
        return 2;
    }
    int width = argc >= 2 ? atoi(argv[1]) : 60;
    int height = argc >= 3 ? atoi(argv[2]) : 24;
    int steps = argc >= 4 ? atoi(argv[3]) : 60;
    if (width < 4)
        width = 4;
    if (height < 4)
        height = 4;
    tmb_world w;
    if (load_world(argv[0], &w) != 0)
        return 1;
    for (int i = 0; i < steps; i++)
        tmb_world_step(&w, 1.0f / 60.0f);
    tmb_buf out;
    tmb_buf_init(&out);
    tmb_err e = tmb_render_ascii(&w, width, height, &out);
    if (e == TMB_OK && out.data)
        fwrite(out.data, 1, out.len, stdout);
    tmb_buf_free(&out);
    tmb_world_free(&w);
    return e == TMB_OK ? 0 : 1;
}

static int cmd_stats(int argc, char **argv)
{
    if (argc < 1) {
        fprintf(stderr, "usage: tumble stats <scene>\n");
        return 2;
    }
    tmb_world w;
    if (load_world(argv[0], &w) != 0)
        return 1;
    tmb_stats s;
    tmb_world_stats(&w, &s);
    tmb_buf out;
    tmb_buf_init(&out);
    tmb_stats_print(&s, &out);
    if (out.data)
        fwrite(out.data, 1, out.len, stdout);
    tmb_buf_free(&out);
    tmb_world_free(&w);
    return 0;
}

static int cmd_convert(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: tumble convert <in> <out>\n");
        return 2;
    }
    tmb_world w;
    if (load_world(argv[0], &w) != 0)
        return 1;
    const char *out = argv[1];
    size_t n = strlen(out);
    int as_bin = (n > 4 && strcmp(out + n - 4, ".bin") == 0);
    tmb_buf buf;
    tmb_buf_init(&buf);
    tmb_err e = as_bin ? tmb_write_scene_bin(&w, &buf)
                       : tmb_write_scene_text(&w, &buf);
    int rc = 0;
    if (e != TMB_OK) {
        fprintf(stderr, "serialize failed: %s\n", tmb_strerror(e));
        rc = 1;
    } else if (write_file(out, buf.data ? buf.data : (const uint8_t *)"",
                          buf.len) != 0) {
        fprintf(stderr, "write '%s' failed\n", out);
        rc = 1;
    } else {
        printf("wrote %zu bytes to %s\n", buf.len, out);
    }
    tmb_buf_free(&buf);
    tmb_world_free(&w);
    return rc;
}

static int cmd_gen(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr,
                "usage: tumble gen <wall|pit|dominoes|scatter> <out> [seed]\n");
        return 2;
    }
    const char *kind = argv[0];
    const char *out = argv[1];
    uint32_t seed = argc >= 3 ? (uint32_t)strtoul(argv[2], NULL, 10) : 1u;

    tmb_world w;
    tmb_world_init(&w);
    w.gravity = tmb_v2(0, -9.8f);
    tmb_build_ground(&w, 40.0f, -10.0f);

    tmb_rng rng;
    tmb_rng_seed(&rng, seed);
    size_t added = 0;
    if (strcmp(kind, "wall") == 0) {
        added = tmb_gen_brick_wall(&w, tmb_v2(-8, -9), 10, 8, 0.8f, 0.4f);
    } else if (strcmp(kind, "pit") == 0) {
        tmb_aabb region = {{-10, -8}, {10, 8}};
        added = tmb_gen_ball_pit(&w, &rng, region, 80, 0.3f, 0.7f);
    } else if (strcmp(kind, "dominoes") == 0) {
        added = tmb_gen_domino_line(&w, tmb_v2(-15, -9), 40, 0.8f);
    } else if (strcmp(kind, "scatter") == 0) {
        tmb_aabb region = {{-10, 0}, {10, 12}};
        added = tmb_gen_scatter_polygons(&w, &rng, region, 60, 3, 7);
    } else {
        fprintf(stderr, "unknown kind '%s'\n", kind);
        tmb_world_free(&w);
        return 2;
    }

    tmb_buf buf;
    tmb_buf_init(&buf);
    tmb_err e = tmb_write_scene_text(&w, &buf);
    int rc = 0;
    if (e != TMB_OK || write_file(out, buf.data, buf.len) != 0) {
        fprintf(stderr, "write '%s' failed\n", out);
        rc = 1;
    } else {
        printf("generated %s with %zu bodies -> %s\n", kind, added, out);
    }
    tmb_buf_free(&buf);
    tmb_world_free(&w);
    return rc;
}

static int cmd_bench(int argc, char **argv)
{
    if (argc < 1) {
        fprintf(stderr, "usage: tumble bench <scene> [steps]\n");
        return 2;
    }
    int steps = argc >= 2 ? atoi(argv[1]) : 1000;
    if (steps < 1)
        steps = 1;
    tmb_world w;
    if (load_world(argv[0], &w) != 0)
        return 1;
    clock_t t0 = clock();
    for (int i = 0; i < steps; i++)
        tmb_world_step(&w, 1.0f / 60.0f);
    clock_t t1 = clock();
    double secs = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
    double sps = secs > 0 ? steps / secs : 0;
    printf("%d steps, %zu bodies in %.4f s -> %.0f steps/s\n", steps,
           tmb_world_body_count(&w), secs, sps);
    tmb_world_free(&w);
    return 0;
}

static int usage(void)
{
    fprintf(stderr,
            "tumble %s\n"
            "usage: tumble <command> [args]\n"
            "  version\n"
            "  run     <scene> [steps] [dt]\n"
            "  render  <scene> [w] [h] [steps]\n"
            "  stats   <scene>\n"
            "  convert <in> <out>\n"
            "  gen     <kind> <out> [seed]\n"
            "  bench   <scene> [steps]\n",
            tmb_version());
    return 2;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return usage();
    const char *cmd = argv[1];
    int rest = argc - 2;
    char **args = argv + 2;
    if (strcmp(cmd, "version") == 0) {
        printf("tumble %s\n", tmb_version());
        return 0;
    }
    if (strcmp(cmd, "run") == 0)
        return cmd_run(rest, args);
    if (strcmp(cmd, "render") == 0)
        return cmd_render(rest, args);
    if (strcmp(cmd, "stats") == 0)
        return cmd_stats(rest, args);
    if (strcmp(cmd, "convert") == 0)
        return cmd_convert(rest, args);
    if (strcmp(cmd, "gen") == 0)
        return cmd_gen(rest, args);
    if (strcmp(cmd, "bench") == 0)
        return cmd_bench(rest, args);
    return usage();
}
