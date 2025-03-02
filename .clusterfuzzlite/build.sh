#!/bin/bash -eu
# Build the Tumble library objects (everything except the CLI front end) and
# link each fuzz target against them.

# Fenrir extracts the repository to the root of $SRC, so build from there.
cd "$SRC"

OBJS=""
for c in src/*.c; do
  case "$c" in
    src/cli.c) continue ;;  # the CLI has main(); not part of the fuzzed library
  esac
  obj="$WORK/$(basename "${c%.c}").o"
  $CC $CFLAGS -Iinclude -Isrc -c "$c" -o "$obj"
  OBJS="$OBJS $obj"
done

for fz in fuzz/*_fuzzer.c; do
  name="$(basename "${fz%.c}")"
  $CC $CFLAGS -Iinclude -Isrc -c "$fz" -o "$WORK/$name.o"
  $CXX $CXXFLAGS $LIB_FUZZING_ENGINE "$WORK/$name.o" $OBJS -lm \
      -o "$OUT/$name"
done

# Seed corpora: zip the matching corpus directory next to each target.
if command -v zip >/dev/null 2>&1; then
  ( cd fuzz/corpus/scene_text && zip -q -r "$OUT/scene_text_fuzzer_seed_corpus.zip" . ) || true
  ( cd fuzz/corpus/scene_bin  && zip -q -r "$OUT/scene_bin_fuzzer_seed_corpus.zip" . ) || true
fi
