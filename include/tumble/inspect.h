#ifndef TUMBLE_INSPECT_H
#define TUMBLE_INSPECT_H

#include "tumble/world.h"
#include "tumble/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Human-readable dumps of world state for debugging and golden-file tests. The
 * output is stable line-oriented text so it diffs cleanly across runs. */

/* One line per body: index, type, pose, velocity, shape summary. */
void tmb_inspect_bodies(const tmb_world *w, tmb_buf *out);
/* One line per joint. */
void tmb_inspect_joints(const tmb_world *w, tmb_buf *out);
/* One line per contact found in the last step. */
void tmb_inspect_contacts(const tmb_world *w, tmb_buf *out);
/* A compact one-line digest (counts and aggregate energy). */
void tmb_inspect_summary(tmb_world *w, tmb_buf *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_INSPECT_H */
