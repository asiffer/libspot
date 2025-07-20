# current directory
CURDIR = .
ROOT = $(CURDIR)/..

VERSION = $(shell grep -om 1 '^VERSION.*' $(ROOT)/Makefile | awk '{print $$3}')

.PHONY: test clean

# ========================================================================== #
# Sources
# ========================================================================== #

# folder of the headers
INC_DIR = $(ROOT)/include
# folder of the sources
SRC_DIR = $(ROOT)/src

# ========================================================================== #
# Compiler stuff
# ========================================================================== #
CC                 := clang
CMOREFLAGS         :=
CBASEFLAGS         := --target=wasm32 -O3 -std=c99 -I$(INC_DIR) -D 'VERSION="$(VERSION)"'
CFLAGS             ?= $(CBASEFLAGS) -Wall -Wextra -Werror -pedantic $(CMOREFLAGS)
LDFLAGS 		   := -nostdlib -Wl,--no-entry -Wl,--export-all

js: dist/libspot.js types/index.d.ts

types/index.d.ts: dist/libspot.js
	@bun run tsc 
	@cd types && find . -maxdepth 1 -type f ! -name 'libspot.d.ts' -delete
	@mv types/libspot.d.ts types/index.d.ts

dist/libspot.js: libspot.ts libspot.core.ts binary.ts
	@bun build --production $< --outdir $(@D)

libspot.wasm: $(SRC_DIR)/*.c main.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ 

binary.ts: libspot.wasm
	@echo "export const wasmBinary = new Uint8Array([" > $@
	@od -t x1 -A n $^ >> $@
	@echo "]);" >> $@
	@sed -i -E 's/([0-9a-f][0-9a-f])/0x\1,/g' $@
	@bun run prettier -w --parser typescript $@

tgz: libspot-$(VERSION).tgz

libspot-$(VERSION).tgz: dist/libspot.js types/index.d.ts
	@bun pm pack

test:
	@bun test libspot.test.ts

clean:
	rm -f libspot.wasm
	rm -f binary.ts
	rm -rf dist/
	rm -rf types/