ROOT_DIR = $(shell realpath $(CURDIR)/..)
# arduino lib
ARDUINO_DIR      = $(CURDIR)
ARDUINO_LIB      = Spot
ARDUINO_LIB_DIR  = $(ARDUINO_DIR)/$(ARDUINO_LIB)
ARDUINO_DIST_DIR = $(ARDUINO_DIR)

LIBSPOT_DIST_DIR = $(ROOT_DIR)/dist

SORTED_HEADERS = $(shell cc -MM $(ROOT_DIR)/include/*.h|sed 's,[a-z0-9]*[.]o[:][ ],,g'|sed ':a;/\\$$/{N;s/\\\n */ /;ba}'|awk '{for(i=2;i<=NF;i++) print $$i, $$1}'|tsort|tr '\n' ' ')
SORTED_SRCS = $(shell echo "$(SORTED_HEADERS)"|sed 's,[.]h,.c,g'|sed 's,include[/],src/,g')

VERSION = $(shell make -s -C $(ROOT_DIR) version)

# ========================================================================== #
# Arduino
# ========================================================================== #

arduino: $(ARDUINO_DIST_DIR)/$(ARDUINO_LIB).zip

install: $(HOME)/Arduino/libraries/$(ARDUINO_LIB)

$(LIBSPOT_DIST_DIR)/spot.h:
	@make -C $(ROOT_DIR) api

$(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).h: $(LIBSPOT_DIST_DIR)/spot.h
	@mkdir -p $(@D)
	@cp -u $< $@

$(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).cpp: $(SORTED_SRCS)
	@mkdir -p $(@D)
	echo '#include "$(ARDUINO_LIB).h"' > $@
	cat $^ | sed 's/^#include ".*h"//g' | sed 's,^[/][/].*,,g' | awk '/^[/][*][*]/{f=1} !f;/^[ ][*][/]/{f=0}' >> $@

$(ARDUINO_DIR)/$(ARDUINO_LIB).zip: $(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).h $(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).cpp $(ARDUINO_LIB_DIR)/library.properties
	@mkdir -p $(@D)
	sed -i $(ARDUINO_LIB_DIR)/library.properties -e 's/^version=.*$$/version=$(VERSION)/g'
	cd $(ARDUINO_DIR) && zip -r $(ARDUINO_LIB).zip $(ARDUINO_LIB)/*

$(HOME)/Arduino/libraries/$(ARDUINO_LIB): $(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).h $(ARDUINO_LIB_DIR)/src/$(ARDUINO_LIB).cpp $(ARDUINO_LIB_DIR)/library.properties
	@mkdir -p $(@D)
	cp -ru $(ARDUINO_LIB_DIR) $@

clean:
	rm -rf $(ARDUINO_LIB_DIR)/src
	rm -f  $(ARDUINO_DIR)/$(ARDUINO_LIB).zip


