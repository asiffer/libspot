### libspot makefile
#
# @author = asr


LIB 		 = libspot
VERSION 	 = 2.0a
COMMIT_COUNT = $(shell git rev-list --count master)

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
# folder of the shared library
LIB_DIR = $(CURDIR)/lib
# folder for the testspot
TEST_DIR = $(CURDIR)/test
# docs output directory
DOXYGEN_DIR = $(CURDIR)/$(shell grep OUTPUT_DIRECTORY Doxyfile|awk -F '=' '{print $$2}')
# test results dir
TEST_RESULTS_DIR = $(BUILD_DIR)/results
TEST_OBJS_DIR = $(BUILD_DIR)/objs
TEST_DEPENDS_DIR = $(BUILD_DIR)/depends
TEST_COVERAGE_DIR = $(CURDIR)/coverage
# Unity directory
UNITY_DIR = $(CURDIR)/unity
# folder of python stuff
PYTHON_DIR = $(CURDIR)/python
# folder of python wheel
PYTHON_DIST_DIR = $(CURDIR)/dist
# header files
HEADERS = $(wildcard $(INC_DIR)/*.h)
# source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# compiled files
OBJS = $(SRCS:$(SRC_DIR)%.c=$(BUILD_DIR)%.o)
# test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
# deps to generate
DEPS = $(TEST_SRCS:$(TEST_DIR)%.c=$(TEST_DEPENDS_DIR)%.d)


# ========================================================================== #
# Destinations
# ========================================================================== #

DESTDIR =
PREFIX = /usr
INSTALL_HEAD_DIR = $(DESTDIR)$(PREFIX)/include/libspot
INSTALL_LIB_DIR = $(DESTDIR)$(PREFIX)/lib
INSTALLED_HEADERS = $(HEADERS:$(INC_DIR)%.h=$(INSTALL_HEAD_DIR)%.h)

EXPORT = LD_LIBRARY_PATH=$(LIB_DIR)

# ========================================================================== #
# Compiler stuff
# ========================================================================== #
CC                 = gcc
CMOREFLAGS         =
CBASEFLAGS         = -O2 -std=c99 -I$(INC_DIR)
CFLAGS             = $(CBASEFLAGS) -Wall -Wextra -Werror -pedantic -D VERSION=\"$(VERSION)-$(COMMIT_COUNT)\" $(CMOREFLAGS)
LDFLAGS            = -static -nostdlib
# CTESTFLAGS         = -O0 -g -fprofile-abs-path -fprofile-arcs -ftest-coverage -Wall -pedantic -std=c++14 --coverage -D VERSION=\"$(VERSION)-$(COMMIT_COUNT)\"
CTESTFLAGS         = $(CBASEFLAGS) -I$(UNITY_DIR) -I$(TEST_DIR) -DTESTING -DUNITY_INCLUDE_DOUBLE -lm -fprofile-abs-path -fprofile-arcs -ftest-coverage -g

# ========================================================================== #
# Targets
# ========================================================================== #
# library file
DYNAMIC ?= $(LIB_DIR)/$(LIB).so.$(VERSION)
STATIC  ?= $(LIB_DIR)/$(LIB).a.$(VERSION)

# ========================================================================== #
# Misc
# ========================================================================== #
# fancyness (green OK)
OK = \t\033[32mOK\033[0m
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
.PHONY: test
.PHONY: doxygen

.DEFAULT:
	@echo -e '\033[31mUnknown command "$@"\033[0m'
	@echo 'Usage: make [command] [variable=]...'
	@echo ''
	@echo 'Commands:'
	@echo '          static    build the static library'
	@echo '         dynamic    build the dynamic library'
	@echo '             all    build both the static and dynamic libs'
	@echo '     install_all    install the headers and the libraries'
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

all: static dynamic

static: $(STATIC)

dynamic: $(DYNAMIC)

install_all: $(INSTALL_LIB_DIR)/$(LIB).a $(INSTALL_LIB_DIR)/$(LIB).so $(INSTALLED_HEADERS)

# create the shared library
$(DYNAMIC): $(OBJS)
	@mkdir -p $(@D)
	@echo -n "Building $(@F)"
	@$(CC) $(CFLAGS) -shared $^ -o $@ -fPIC $(LDFLAGS)
	@echo -e "$(OK)"

$(STATIC): $(OBJS)
	@mkdir -p $(@D)
	@echo -n "Building $(@F)"
	@ar rcs $@ $^
	@echo -e "$(OK)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo -n "Building $(@F)"
	@$(CC) $(CFLAGS) $(EXT_INC_DIR) -c $< -o $@ -fPIC 
	@echo -e "$(OK)"

$(INSTALL_HEAD_DIR)/%: $(INC_DIR)/%
	@mkdir -p $(@D)
	@echo -n "Installing $(@F) to $@"
	@install $< $@
	@echo -e "$(OK)"

$(INSTALL_LIB_DIR)/%.$(VERSION): $(LIB_DIR)/%.$(VERSION)
	@mkdir -p $(@D)
	@echo -n "Installing $(@F) to $@"
	@install $< $@
	@echo -e "$(OK)"

$(INSTALL_LIB_DIR)/%.so: $(INSTALL_LIB_DIR)/%.so.$(VERSION)
	@echo -n "Linking $@ -> $<"
	@ln -s $< $@
	@echo -e "$(OK)"

$(INSTALL_LIB_DIR)/%.a: $(INSTALL_LIB_DIR)/%.a.$(VERSION)
	@echo -n "Linking $@ -> $<"
	@ln -s $< $@
	@echo -e "$(OK)"


## HTML/XML docs
doxygen:
	@rm -rf $(DOXYGEN_DIR)
	@doxygen

dev/doxygen/generated: doxygen
	mkdir -p $(@D)
	xsdata generate doxygen/xml/
	mv generated $@

## CLEAN
clean:
	rm -f $(OBJS)
	rm -f $(STATIC) $(DYNAMIC)
	rm -f $(TEST_OBJS_DIR)/*.o
	rm -f $(TEST_OBJS_DIR)/*.gcda $(TEST_OBJS_DIR)/*.gcno
	rm -f $(TEST_RESULTS_DIR)/*.txt
	-rm -df $(BUILD_DIR)/*
	rm -rf $(TEST_COVERAGE_DIR)
	rm -f .coverage.info
	rm -rf $(DOXYGEN_DIR)
	

old_clean:
	# @rm -rfd $(BUILD_DIR)
	# @rm -rfd $(LIB_DIR)
	# @rm -rfd $(PYTHON_DIST_DIR)
	# @cd $(PYTHON_DIR) && rm -rf $$(ls -I "*.py" .)
	# @cd $(TEST_DIR) && rm -f $$(ls -I "*.cpp" .)
	# @rm -rfd ./debian/*.log ./debian/*.substvars ./debian/tmp ./debian/.deb*

python:
	@python3 -m build -nw



# Unity testing ==============================================================

# required paths to build test stuff
BUILD_PATHS = $(BUILD_DIR) $(TEST_DEPENDS_DIR) $(TEST_OBJS_DIR) $(TEST_RESULTS_DIR)
# test outputs
RESULTS = $(patsubst $(TEST_DIR)/%_test.c,$(TEST_RESULTS_DIR)/%_test.txt,$(TEST_SRCS))

# formatting fancyness
RESULT_FMT := "%23s | %4s | %30s | %-7s\n"

# categorize results base on PASS/FAIL/IGNORE
PASSED = `grep -s :PASS   $(TEST_RESULTS_DIR)/*.txt | awk -F ':' '{printf $(subst %-7s,\033[92m%-7s\033[0m,$(RESULT_FMT)),$$2,$$3,$$4,$$5}'`
FAIL   = `grep -s :FAIL   $(TEST_RESULTS_DIR)/*.txt | awk -F ':' '{printf $(subst %-7s,\033[91m%-7s\033[0m,$(RESULT_FMT)),$$2,$$3,$$4,$$5}'`
IGNORE = `grep -s :IGNORE $(TEST_RESULTS_DIR)/*.txt | awk -F ':' '{printf $(subst %-7s,\033[93m%-7s\033[0m,$(RESULT_FMT)),$$2,$$3,$$4,$$5}'`

RESULTS_TABLE = $(FAIL)$(IGNORE)$(PASSED)

test: $(BUILD_PATHS) $(RESULTS)
	@printf $(RESULT_FMT) "File" "Line" "Function" "Result"
	@printf $(RESULT_FMT) " " " " " " " " | tr ' ' '-'
	@echo -e "$(RESULTS_TABLE)"


depends: $(BUILD_PATHS) $(DEPS)

$(TEST_RESULTS_DIR)/%.txt: $(BUILD_DIR)/%
	@echo -n "Generating $(@F)"
	@-$< > $@ 2>&1
	@echo -e "$(OK)"

$(BUILD_DIR)/%_test: $(TEST_DEPENDS_DIR)/%_test.d $(BUILD_DIR) $(TEST_OBJS_DIR)

$(TEST_OBJS_DIR)/%.o:: $(TEST_DIR)/%.c
	@echo -n "Building $(@F)"
	@$(CC) $(CTESTFLAGS) -o $@ -c $<
	@echo -e "$(OK)"

$(TEST_OBJS_DIR)/%.o:: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	@echo -n "Building $(@F)"
	@$(CC) $(CTESTFLAGS) -o $@ -c $<
	@echo -e "$(OK)"

$(TEST_OBJS_DIR)/%.o:: $(UNITY_DIR)/%.c $(UNITY_DIR)/%.h
	@echo -n "Building $(@F)"
	@$(CC) $(CTESTFLAGS) -o $@ -c $<
	@echo -e "$(OK)"

$(TEST_DEPENDS_DIR)/%.d : $(TEST_DIR)/%.c
	$(CC) $(CTESTFLAGS) -MMD -MG -MM -MT"$(BUILD_DIR)/$*" -o "$@" "$<"
	@sed -i -E -e 's,(include|test|unity)/,build/objs/,g' "$@"
	@sed -i -E -e 's,\.[hc],\.o,g' "$@"
	@sed -i -E -e 's,build/objs/test[a-zA-Z_]+\.o,,g' "$@"
	@sed -i -e 's,build/objs/unity_internals.o,,g' "$@"
	@echo '	@echo -n "Linking $$(@F)"' >> "$@"
	@echo '	@$(CC) $(CTESTFLAGS) -o $(BUILD_DIR)/$* $$^' >> "$@"
	@echo '	@echo -e "$(OK)"' >> "$@"
	
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TEST_DEPENDS_DIR):
	@mkdir -p $(TEST_DEPENDS_DIR)

$(TEST_OBJS_DIR):
	@mkdir -p $(TEST_OBJS_DIR)

$(TEST_RESULTS_DIR):
	@mkdir -p $(TEST_RESULTS_DIR)


-include $(TEST_SRCS:$(TEST_DIR)%.c=$(TEST_DEPENDS_DIR)%.d)


.coverage.info: test
	@lcov --capture --directory $(TEST_OBJS_DIR) --output-file "$@" 

$(TEST_COVERAGE_DIR): .coverage.info
	@genhtml "$^" --output-directory "$@"

coverage: $(TEST_COVERAGE_DIR)

coverage-serve: coverage
	@python3 -m http.server -d $(TEST_COVERAGE_DIR) -b 127.0.0.1 8000