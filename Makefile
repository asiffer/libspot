# 
#  ██╗     ██╗██████╗ ███████╗██████╗  ██████╗ ████████╗
#  ██║     ██║██╔══██╗██╔════╝██╔══██╗██╔═══██╗╚══██╔══╝
#  ██║     ██║██████╔╝███████╗██████╔╝██║   ██║   ██║   
#  ██║     ██║██╔══██╗╚════██║██╔═══╝ ██║   ██║   ██║   
#  ███████╗██║██████╔╝███████║██║     ╚██████╔╝   ██║   
#  ╚══════╝╚═╝╚═════╝ ╚══════╝╚═╝      ╚═════╝    ╚═╝   
#                                                       


LIB 		 = libspot
VERSION 	 = 2.0b0
LICENSE 	 = GNU Lesser General Public License v3.0
DATE         = $(shell date -u)
COMMIT_COUNT = $(shell git rev-list --count master)

ARCH         = $(shell uname -m)

# ========================================================================== #
# Binaries
# ========================================================================== #

PODMAN = podman
SHELL  = /bin/sh

# ========================================================================== #
# Sources
# ========================================================================== #

# current directory
CURDIR = .
# folder of the headers
INC_DIR = $(CURDIR)/include
# folder of the sources
SRC_DIR = $(CURDIR)/src
#folder of the built sources
BUILD_DIR = $(CURDIR)/build
# output folder
DIST_DIR = $(CURDIR)/dist
# docs output directory
DOXYGEN_DIR = $(CURDIR)/$(shell grep OUTPUT_DIRECTORY doxyfile|awk -F '=' '{print $$2}')
# test directories
TEST_DIR          = $(CURDIR)/test
TEST_SRC_DIR      = $(TEST_DIR)/src
TEST_RESULTS_DIR  = $(TEST_DIR)/results
TEST_BIN_DIR      = $(TEST_DIR)/bin
TEST_BUILD_DIR    = $(TEST_DIR)/build
TEST_DEPENDS_DIR  = $(TEST_DIR)/deps
TEST_COVERAGE_DIR = $(TEST_DIR)/coverage
# benchmark dir
BENCHMARK_DIR = $(CURDIR)/benchmark
# Unity directory
UNITY_DIR = $(CURDIR)/unity
# folder of python stuff
PYTHON_DIR = $(CURDIR)/python
# folder of python wheel
PYTHON_DIST_DIR = $(PYTHON_DIR)/dist
# local python version (like 311 or 312)
PYTHON_VERSION = $(shell python -V|awk '{print $2}'|awk -F '.' '{print $1$2}')
WHEEL_TAG = $(shell python -c 'from wheel import bdist_wheel as bw; abi=bw.get_abi_tag(); print("-".join([abi, abi, bw.get_platform(None)]))')
# emscripten compiler
EMCC = $(shell command -pv emcc)
ifndef $(EMCC)
	EMCC=podman run --rm -v $(shell pwd):/src emscripten/emsdk:3.1.46 emcc
endif

# wasm folder
WASM_DIR = $(CURDIR)/wasm
# arduino lib
ARDUINO_DIR = $(CURDIR)/arduino
ARDUINO_LIB = spot
# header files
HEADERS = $(wildcard $(INC_DIR)/*.h)
SORTED_HEADERS = $(shell $(CC) -MM include/spot.h | grep -oE "include/.*.h" | tr '\n' ' ' | tac -s ' ')
# source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
SORTED_SRCS = $(SORTED_HEADERS:include/%.h=src/%.c)
# compiled files
OBJS = $(SRCS:$(SRC_DIR)%.c=$(BUILD_DIR)%.o)
# test files
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.c)
# deps to generate
DEPS = $(TEST_SRCS:$(TEST_SRC_DIR)%.c=$(TEST_DEPENDS_DIR)%.d)
TEST_RESULTS = $(TEST_SRCS:$(TEST_SRC_DIR)%.c=$(TEST_RESULTS_DIR)%.txt)


# ========================================================================== #
# Destinations
# ========================================================================== #

DESTDIR =
PREFIX = /usr
INSTALL_HEAD_DIR = $(DESTDIR)$(PREFIX)/include/spot
INSTALL_LIB_DIR = $(DESTDIR)$(PREFIX)/lib
INSTALLED_HEADERS = $(HEADERS:$(INC_DIR)%.h=$(INSTALL_HEAD_DIR)%.h)


# ========================================================================== #
# Compiler stuff
# ========================================================================== #
CC                 ?= cc
CMOREFLAGS         :=
CBASEFLAGS         := -O3 -std=c99 -I$(INC_DIR) -D 'VERSION="$(VERSION)"'
CFLAGS             ?= $(CBASEFLAGS) -Wall -Wextra -Werror -pedantic $(CMOREFLAGS)
LDFLAGS            ?= -static -nostdlib
CTESTFLAGS         := $(CBASEFLAGS) -I$(UNITY_DIR) -I$(TEST_DIR) -DTESTING -DUNITY_INCLUDE_DOUBLE -lm -fprofile-arcs -ftest-coverage -g

# ========================================================================== #
# Other constants
# ========================================================================== #
# number of run to perform
BENCHMARK_COUNT := 200
# number of data for each run
BENCHMARK_SIZE	:= 10000000

rand = $(shell head -c 4 /dev/urandom|od -DAn|sed 's, ,,g')
# ========================================================================== #
# Targets
# ========================================================================== #
# library file
DYNAMIC ?= $(DIST_DIR)/$(LIB).so.$(VERSION)
STATIC  ?= $(DIST_DIR)/$(LIB).a.$(VERSION)

# ========================================================================== #
# Misc
# ========================================================================== #
# fancyness (green OK)
OK = \t\033[32mOK\033[0m
PASS   = $(shell printf "\033[92mPASS\033[0m")
FAIL   = $(shell printf "\033[91mFAIL\033[0m")
IGNORE = $(shell printf "\033[93mIGNORE\033[0m")

# custom functions

# apify removes the file header of C source code, 
# line starting by '#' and empty lines
define apify
    tail -n $$(grep -nm1 '*/' $(1) | awk -F ':' '{print "+"$$1+1}') $(1) | sed -e 's,^#.*$$,,g' | sed '/^$$/N;/^\n$$/D'
endef

# when no command is specified, build all
.DEFAULT_GOAL := all
# we must keep the library with appended version
# as final libraries point to them
.PRECIOUS: $(INSTALL_LIB_DIR)/%.$(VERSION) 
.PRECIOUS: $(BUILD_DIR)/%_test
.PRECIOUS: $(TEST_DEPENDS_DIR)/%.d
.PRECIOUS: $(TEST_OBJS_DIR)/%.o
.PRECIOUS: $(TEST_RESULTS_DIR)/%.txt

.PHONY: static
.PHONY: dynamic
.PHONY: clean
.PHONY: deps
.PHONY: test
.PHONY: check
.PHONY: doxygen
.PHONY: wheel

.DEFAULT:
	@echo -e '\033[31mUnknown command "$@"\033[0m'
	@echo 'Usage: make [command] [variable=]...'
	@echo ''
	@echo 'Commands:'
	@echo '          static    build the static library'
	@echo '         dynamic    build the dynamic library'
	@echo '             all    build both the static and dynamic libs'
	@echo '             api    build libspot API header dist/spot.h'
	@echo '         install    install the headers and the libraries'
	@echo '           clean    remove the build artifacts'
	@echo '         version    print the library version'
	@echo '    version_full    print the library version + commit count'
	@echo ''
	@echo 'Variables:'
	@echo '         DESTDIR    installation root directory'
	@echo '          PREFIX    installation prefix directory'

version:
	@echo $(VERSION)

version_full:
	@echo "$(VERSION)-$(COMMIT_COUNT)"

all: static dynamic api

static: $(STATIC)

dynamic: $(DYNAMIC)

install: $(INSTALL_LIB_DIR)/$(LIB).a $(INSTALL_LIB_DIR)/$(LIB).so $(INSTALLED_HEADERS)

uninstall:
	rm -rf $(INSTALL_LIB_DIR)/$(LIB).* $(INSTALLED_HEADERS)

# ========================================================================== #
# Distribute
# ========================================================================== #

$(DYNAMIC): $(OBJS)
	@mkdir -p $(@D)
	@printf "%-25s" "LINK $(@F)"
	@$(CC) $(CFLAGS) -shared $^ -o $@ -fPIC $(LDFLAGS)
	@echo -e "$(OK)"

$(STATIC): $(OBJS)
	@mkdir -p $(@D)
	@printf "%-25s" "AR   $(@F)"
	@ar rcs $@ $^
	@echo -e "$(OK)"

$(DIST_DIR)/spot.h: $(HEADERS)
	@mkdir -p $(@D)
	@$(call apify,include/structs.h) > $@
	@$(call apify,include/spot.h) >> $@
	@clang-format --style=file -i $@


api: $(DIST_DIR)/spot.h

# ========================================================================== #
# Build
# ========================================================================== #

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@mkdir -p $(@D)
	@printf "%-25s" "CC   $(@F)"
	@$(CC) $(CFLAGS) $(EXT_INC_DIR) -c $< -o $@ -fPIC 
	@echo -e "$(OK)"


# ========================================================================== #
# Install
# ========================================================================== #

$(INSTALL_HEAD_DIR)/%: $(INC_DIR)/%
	@mkdir -p $(@D)
	@printf "INSTALL %-25s %-30s" "$(@F)" "$@"
	@install $< $@
	@echo "$(OK)"

$(INSTALL_LIB_DIR)/%.$(VERSION): $(DIST_DIR)/%.$(VERSION)
	@mkdir -p $(@D)
	@printf "INSTALL %-25s %-30s" "$(@F)" "$@"
	@install $< $@
	@echo "$(OK)"

$(INSTALL_LIB_DIR)/%.so: $(INSTALL_LIB_DIR)/%.so.$(VERSION)
	@printf "SYMLINK %-25s %-30s" "$<" "$@"
	@ln -s $< $@
	@echo "$(OK)"

$(INSTALL_LIB_DIR)/%.a: $(INSTALL_LIB_DIR)/%.a.$(VERSION)
	@printf "SYMLINK %-25s %-30s" "$<" "$@"
	@ln -s $< $@
	@echo "$(OK)"

# ========================================================================== #
# Docs
# ========================================================================== #

doxygen: $(DIST_DIR)/spot.h
	@rm -rf $(DOXYGEN_DIR)
	@doxygen

dev/doxygen/generated: doxygen
	@mkdir -p $(@D) && rm -rf $@
	@xsdata generate doxygen/xml/
	@mv -f generated $@

inject-version: $(HEADERS) $(SRCS)
	@printf "%-55s" "Setting version to '$(VERSION)'"
	@sed -i -e 's,@version.*,@version $(VERSION),' $^
	@echo -e "$(OK)"

inject-date: $(HEADERS) $(SRCS)
	@printf "%-55s" "Setting date to '$(DATE)'"
	@sed -i -e 's,@date.*,@date $(DATE),' $^
	@echo -e "$(OK)"

inject-copyright: $(HEADERS) $(SRCS)
	@printf "%-55s" "Setting license to '$(LICENSE)'"
	@sed -i -e 's,@copyright.*,@copyright $(LICENSE),' $^
	@echo -e "$(OK)"

docs/70_API.md: dev/doxygen/generated
	@mkdir -p $(@D)
	python3 dev/doxygen/generate_api_docs.py -o "$@"

docs/API: docs/70_API.md


# ========================================================================== #
# Misc
# ========================================================================== #

libspot.tar.gz:
	@tar -cvf $@ src include Makefile benchmark/*.c 

check:
	@clang-tidy $(SRC_DIR)/*.c -- $(CFLAGS)

fmt:
	@clang-format -i $(SRC_DIR)/*.c $(INC_DIR)/*.h $(DIST_DIR)/spot.h

clean:
	rm -f $(OBJS)
	rm -f $(STATIC) $(DYNAMIC)
	rm -rf $(TEST_COVERAGE_DIR) $(TEST_RESULTS_DIR) $(TEST_BIN_DIR)
	rm -rf $(DOXYGEN_DIR)
	rm -rf dev/doxygen/generated
	rm -rf $(BENCHMARK_DIR)/bin
	rm -rf docs/API
	rm -rf $(PYTHON_DIR)/build $(PYTHON_DIR)/dist $(PYTHON_DIR)/$(LIB).egg-info
	rm -rf $(PYTHON_DIR)/$(LIB)/interface.py
	rm -rf $(WASM_DIR)/dist
	rm -rf $(WASM_DIR)/libspot.js
	

# ========================================================================== #
# Python
# ========================================================================== #


$(PYTHON_DIR)/dist/libspot-$(VERSION)-$(WHEEL_TAG).whl: $(PYTHON_DIR)/setup.py $(PYTHON_DIR)/libspotmodule.c $(SRC_DIR)/*.c
	@mkdir -p $(@D)
	python -m build -w -o $(PYTHON_DIR)/dist $(PYTHON_DIR)

python: $(PYTHON_DIR)/dist/libspot-$(VERSION)-$(WHEEL_TAG).whl

wheel: python

python3.%:
	podman run --rm -it -v $(shell pwd):/libspot -w /libspot python:3.$* python -m build -w -o $(PYTHON_DIR)/dist $(PYTHON_DIR)

python-all: $(foreach v,11 12,python3.$(v))

# ========================================================================== #
# WASM/JS
# ========================================================================== #

js: $(WASM_DIR)/dist/libspot.js

$(WASM_DIR)/dist/libspot.js: $(WASM_DIR)/libspot.core.js 
	cd $(WASM_DIR) && bun run build

$(WASM_DIR)/libspot.core.js: $(SRC_DIR)/*.c $(WASM_DIR)/main.c 
	$(EMCC) $(CFLAGS) \
		-s WASM=1 \
		-s FILESYSTEM=0 \
		-s EXPORT_ES6=1 \
		-s MODULARIZE=1 \
		-s SINGLE_FILE \
		-s ENVIRONMENT=web \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s EXPORTED_RUNTIME_METHODS=cwrap,ccall \
		-s EXPORT_NAME=loadWASM \
		-s NO_EXIT_RUNTIME=1 \
		-s EXPORTED_FUNCTIONS=_spot_size,_spot_new,_spot_init,_spot_fit,_spot_step,_spot_quantile,_spot_probability,_spot_free,_libspot_error,_libspot_version,_malloc,_free,_set_allocators,_main \
		-o $@ $^


# ========================================================================== #
# Arduino
# ========================================================================== #


$(ARDUINO_DIR)/$(ARDUINO_LIB)/$(ARDUINO_LIB).h: $(DIST_DIR)/spot.h
	@mkdir -p $(@D)
	@cp -u $< $@

$(ARDUINO_DIR)/$(ARDUINO_LIB)/$(ARDUINO_LIB).cpp: $(SORTED_SRCS)
	@mkdir -p $(@D)
	echo '#include "$(ARDUINO_LIB).h"' > $@
	@for i in $^; do sed 's/^#include ".*h"//g' $$i | awk '/^[/][*][*]/{c++} c!=1; /^ [*][/]/{c++}' >> $@; done

$(ARDUINO_DIR)/$(ARDUINO_LIB).zip: $(ARDUINO_DIR)/$(ARDUINO_LIB)/$(ARDUINO_LIB).h $(ARDUINO_DIR)/$(ARDUINO_LIB)/$(ARDUINO_LIB).cpp
	cd $(ARDUINO_DIR) && zip -r $(ARDUINO_LIB).zip $(ARDUINO_LIB)/*

arduino: $(ARDUINO_DIR)/$(ARDUINO_LIB).zip

# ========================================================================== #
# Test
# ========================================================================== #

# generate depends for test files
$(TEST_DEPENDS_DIR)/%_test.d: $(TEST_SRC_DIR)/%_test.c $(UNITY_DIR)/unity.c $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@mkdir -p $(@D)
	echo "$$($(CC) -MT $(TEST_BIN_DIR)/$*_test -MM -MG $(INC_DIR)/$*.h|sed -e 's,include/,src/,g' -e 's,[.]h,.c,g') $< $(UNITY_DIR)/unity.c" > "$@"

deps: $(DEPS)

# include that rules
-include $(TEST_SRCS:$(TEST_SRC_DIR)%.c=$(TEST_DEPENDS_DIR)%.d)

# create a generic rule that compile all the depends
$(TEST_BIN_DIR)/%_test: 
	@mkdir -p $(TEST_COVERAGE_DIR) $(TEST_BIN_DIR)
	@printf "%-32s" "Building $@"
	@$(CC) $(CTESTFLAGS) -o $@ $^
	@mv $@-*.gcno $(TEST_COVERAGE_DIR)
	@echo -e "$(OK)"

# run a test
$(TEST_RESULTS_DIR)/%_test.txt: $(TEST_BIN_DIR)/%_test
	@mkdir -p $(@D)
	@printf "%-32s" "Running  $^"
	@$< > "$@"
	@mv $<-*.gcda $(TEST_COVERAGE_DIR)
	@echo -e "$(OK)"

# concat coverage files
$(TEST_COVERAGE_DIR)/coverage.info: $(TEST_RESULTS)
	@lcov -q --capture --directory $(TEST_COVERAGE_DIR) --include '*src*' --exclude '*test*' --output-file $@
	@lcov --list $@

# html display
$(TEST_COVERAGE_DIR)/html: $(TEST_COVERAGE_DIR)/coverage.info
	@genhtml $< --output-directory $@

test: $(TEST_RESULTS)
	@cat $^ | \
		grep -E 'PASS|FAIL|IGNORE' | \
		awk -F ':' '{ printf "%6s %28s %s:%s\n",$$4,$$3,$$1,$$2 }' | \
		sed -e 's,PASS,$(PASS),g' -e 's,FAIL,$(FAIL),g' -e 's,IGNORE,$(IGNORE),g'
	
coverage: $(TEST_COVERAGE_DIR)/html


# ========================================================================== #
# Benchmarks
# ========================================================================== #

$(BENCHMARK_DIR)/bin/%: $(BENCHMARK_DIR)/%.c $(SRCS)
	@mkdir -p $(@D)
	@printf "%-25s" "CC   $(@F)"
	@$(CC) $(CBASEFLAGS) -o "$@" $^ -lm
	@echo -e "$(OK)"

benchmark_%: $(BENCHMARK_DIR)/bin/%
	@printf "%-25s\n" "RUN  $(@F)"
	@for i in $$(seq 1 $(BENCHMARK_COUNT)); do \
		$^ $(BENCHMARK_SIZE) $$(date +%N|sed s/...$$//) >> $(BENCHMARK_DIR)/$*.json; \
	done




