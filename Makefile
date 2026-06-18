#  NumPy C Library — Makefile
#  Targets:  lib | shared | all | test | runtest | clean | install | demo | help

CC       ?= gcc
AR       ?= ar
PREFIX   ?= /usr/local

# ─── Source directories ──────────────────────────────────────────
SRC_DIRS = src/_core src/_core/func src/_core/math src/_core/linalg \
           src/_core/fromnumeric src/_core/shape_base \
           src/_core/function_basic src/_core/io src/_core/npzfile \
           src/_core/numeric src/_core/poly src/_core/multiarray \
           src/_core/stride_tricks src/_core/towdim_base \
           src/_core/utils_func src/_utils src/_utils/sort \
           src/math src/fft src/random src/shape src/features \
           src/datetime64 src/datetime64/compare \
           src/functional/function_base src/functional/window \
           src/functional/histogram src/matrixlib/defmatrix

SRCS      = $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
OBJS      = $(SRCS:src/%.c=build/%.o)
OBJS_PIC  = $(SRCS:src/%.c=build-pic/%.o)

# ─── Platform detection ──────────────────────────────────────────
UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  SHARED_EXT = dylib
  SHARED_FLAG = -dynamiclib -install_name @rpath/$(notdir $(LIB_DIR)/libnpc.$(SHARED_EXT))
  ifneq ($(wildcard /opt/homebrew/lib/libzip.*),)
    LIBZIP_CFLAGS = -I/opt/homebrew/include
    LIBZIP_LIBS   = -L/opt/homebrew/lib -lzip -Wl,-rpath,/opt/homebrew/lib
  else
    LIBZIP_CFLAGS = -I/usr/local/include
    LIBZIP_LIBS   = -L/usr/local/lib -lzip -Wl,-rpath,/usr/local/lib
  endif
else
  SHARED_EXT = so
  SHARED_FLAG = -shared -Wl,-soname,libnpc.so
  LIBZIP_LIBS = -lzip
endif

LIB_DIR       = lib
STATIC_LIB    = $(LIB_DIR)/libnpc.a
SHARED_LIB    = $(LIB_DIR)/libnpc.$(SHARED_EXT)

CFLAGS        = -std=c99 -O2 -Wall -Wextra -Iinclude -Isrc -Itest $(LIBZIP_CFLAGS)
CFLAGS_PIC    = $(CFLAGS) -fPIC
LDFLAGS       = $(LIBZIP_LIBS) -lm

TEST_SRCS     = $(filter-out test/test_utils.c,$(wildcard test/*.c))
TEST_BINS     = $(TEST_SRCS:test/%.c=build/%)
TEST_BINS_SHARED = $(TEST_SRCS:test/%.c=build-shared/%)

# ─── Targets ─────────────────────────────────────────────────────
.PHONY: all lib shared test test-shared runtest clean install help demo

.DEFAULT_GOAL := lib

help:
	@echo "NumPy C Library"
	@echo "  make lib           Build static library  ($(STATIC_LIB))    [default]"
	@echo "  make shared        Build shared library  ($(SHARED_LIB))"
	@echo "  make all           Build static + shared + test executables"
	@echo "  make test          Build and run all tests (static link)"
	@echo "  make test-shared   Build and run all tests (shared link)"
	@echo "  make runtest       Run tests (skip rebuild)"
	@echo "  make demo          Build and run the example"
	@echo "  make clean         Remove build/ build-pic/ build-shared/ lib/"
	@echo "  make install       Install static + shared + headers to PREFIX=$(PREFIX)"

all: lib shared $(TEST_BINS) $(TEST_BINS_SHARED)

lib: $(STATIC_LIB)

shared: $(SHARED_LIB)

# ─── Static library ──────────────────────────────────────────────
$(STATIC_LIB): $(OBJS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Shared library (PIC) ────────────────────────────────────────
$(SHARED_LIB): $(OBJS_PIC)
	@mkdir -p $(LIB_DIR)
	$(CC) $(SHARED_FLAG) $^ $(LDFLAGS) -o $@
	@echo "  SHARED  $@"

build-pic/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_PIC) -c $< -o $@

# ─── Test executables ────────────────────────────────────────────
$(TEST_BINS): build/%: test/%.c test/test_utils.c $(STATIC_LIB)
	@mkdir -p build
	$(CC) $(CFLAGS) $< test/test_utils.c $(STATIC_LIB) $(LDFLAGS) -o $@

$(TEST_BINS_SHARED): build-shared/%: test/%.c test/test_utils.c $(SHARED_LIB)
	@mkdir -p build-shared
	$(CC) $(CFLAGS) $< test/test_utils.c $(SHARED_LIB) $(LDFLAGS) -Wl,-rpath,@executable_path/../lib -o $@

demo: $(STATIC_LIB)
	$(CC) $(CFLAGS) test/example.c $(STATIC_LIB) $(LDFLAGS) -o build/demo
	./build/demo

# ─── Test runner ─────────────────────────────────────────────────
test: $(TEST_BINS)
	@PASS=0; FAIL=0; \
	for t in $(TEST_BINS); do \
	  n=$$(basename $$t); \
	  if $$t >/dev/null 2>&1; then PASS=$$((PASS+1)); \
	  else echo "  FAIL  $$n"; FAIL=$$((FAIL+1)); fi; \
	done; \
	echo "===== $$((PASS+FAIL)) tests: $$PASS pass, $$FAIL fail ====="

test-shared: $(TEST_BINS_SHARED)
	@PASS=0; FAIL=0; \
	for t in $(TEST_BINS_SHARED); do \
	  n=$$(basename $$t); \
	  if $$t >/dev/null 2>&1; then PASS=$$((PASS+1)); \
	  else echo "  FAIL  $$n"; FAIL=$$((FAIL+1)); fi; \
	done; \
	echo "===== $$((PASS+FAIL)) tests: $$PASS pass, $$FAIL fail ====="

runtest:
	@PASS=0; FAIL=0; \
	for t in $(TEST_BINS); do \
	  n=$$(basename $$t); \
	  if [ -x "$$t" ]; then \
	    if $$t >/dev/null 2>&1; then PASS=$$((PASS+1)); \
	    else echo "  FAIL  $$n"; FAIL=$$((FAIL+1)); fi; \
	  fi; \
	done; \
	echo "===== $$((PASS+FAIL)) tests: $$PASS pass, $$FAIL fail ====="

# ─── Install ─────────────────────────────────────────────────────
install: $(STATIC_LIB) $(SHARED_LIB)
	@mkdir -p $(PREFIX)/lib $(PREFIX)/include/npy
	cp $(STATIC_LIB) $(PREFIX)/lib/
	cp $(SHARED_LIB) $(PREFIX)/lib/
	cp include/*.h $(PREFIX)/include/npy/
	@echo "Installed to $(PREFIX)"
	@echo "  $(PREFIX)/lib/libnpc.a"
	@echo "  $(PREFIX)/lib/libnpc.$(SHARED_EXT)"
	@echo "  $(PREFIX)/include/npy/*.h"

# ─── Clean ───────────────────────────────────────────────────────
clean:
	rm -rf build build-pic build-shared $(LIB_DIR)
