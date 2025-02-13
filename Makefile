# Tumble build. `make` builds the static library and the CLI; `make test` runs
# the unit tests; `make fuzz` builds the fuzz targets with libFuzzer + ASan.

CC      ?= cc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra -Wshadow
IFLAGS   = -Iinclude -Isrc
LDFLAGS  = -lm

SRC      = $(filter-out src/cli.c,$(wildcard src/*.c))
OBJ      = $(SRC:.c=.o)
LIB      = libtumble.a
BIN      = tumble

.PHONY: all clean test fuzz examples

all: $(LIB) $(BIN)

$(LIB): $(OBJ)
	$(AR) rcs $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(BIN): src/cli.c $(LIB)
	$(CC) $(CFLAGS) $(IFLAGS) $< $(LIB) $(LDFLAGS) -o $@

TESTS = $(wildcard tests/test_*.c)
test: $(LIB)
	@set -e; for t in $(TESTS); do \
	  echo "  CC   $$t"; \
	  $(CC) $(CFLAGS) $(IFLAGS) $$t $(LIB) $(LDFLAGS) -o $${t%.c}.run; \
	  echo "  RUN  $$t"; $${t%.c}.run; \
	done; echo "all tests passed"

EXAMPLES = $(wildcard examples/*.c)
examples: $(LIB)
	@set -e; for e in $(EXAMPLES); do \
	  $(CC) $(CFLAGS) $(IFLAGS) $$e $(LIB) $(LDFLAGS) -o $${e%.c}; done

fuzz:
	@for f in fuzz/*_fuzzer.c; do \
	  name=$$(basename $${f%.c}); \
	  clang $(CFLAGS) -g -fsanitize=address,fuzzer $(IFLAGS) \
	    $(SRC) $$f $(LDFLAGS) -o build/$$name; done

clean:
	$(RM) $(OBJ) $(LIB) $(BIN) tests/*.run examples/*.run
	$(RM) -r build/*.o
