### libspot makefile
#
# @author = asr

### INSTALL DIRECTORIES
INSTALL_HEAD_DIR = /usr/include/libspot/
INSTALL_LIB_DIR = /usr/lib/
###

# PYTHON : external header (Python.h)
EXT_INC_DIR = ./#/usr/include/python3.5m/




VERSION = 1.0

# Current directory
DIR = $(realpath .)

# folder of the headers
INC_DIR = $(DIR)/include/

# folder of the sources
SRC_DIR = $(DIR)/src/

#folder of the built sources
OBJ_DIR = $(DIR)/build/

#folder of the shared library
LIB_DIR = $(DIR)/lib/

#folder for the testspot
TEST_DIR = $(DIR)/test/


### DEAFULT INSTALL DIRECTORIES
ifeq ($(INSTALL_HEAD_DIR),)
	INSTALL_HEAD_DIR = $(INC_DIR)
endif

ifeq ($(INSTALL_LIB_DIR),)
	INSTALL_LIB_DIR = $(LIB_DIR)
endif
###

EXPORT = export LD_LIBRARY_PATH=$(INSTALL_LIB_DIR)

# compiler & flags
CC = g++-5
CXXFLAGS = -std=c++14 -Wall -pedantic 



# all the files (header, sources, build)
FILES = streammoments.h streamstats.h bounds.h ubend.h brent.h gpdfit.h spot.h dspot.h
DEPS = $(foreach n,$(FILES),$(INC_DIR)$(n))
SRCS = $(foreach n,$(FILES:.h=.cpp),$(SRC_DIR)$(n))
OBJS = $(FILES:.h=.o)

# library file
TARGET = libspot.so.$(VERSION)



### MAKEFILE TARGETS
all: checkdir $(TARGET)

# create lib/ and build/ directories
checkdir:
	@mkdir -p $(LIB_DIR) $(OBJ_DIR)
	@echo -- Building sources --

# create the shared library
$(TARGET): $(OBJS)
	@echo
	@echo -- Building Shared Library --
	$(CC) $(CXXFLAGS) -shared $(foreach n,$^,$(OBJ_DIR)$(n)) -o $(LIB_DIR)$@ -fPIC;

# build source files
%.o: $(SRC_DIR)%.cpp
	$(CC) $(CXXFLAGS) -I $(EXT_INC_DIR) -I $(INC_DIR) -c $< -o $(OBJ_DIR)$@ -fPIC

# install
install:
	# remove a previous link to libspot.so
	rm -f $(INSTALL_LIB_DIR)libspot.so 	
	# link the library			
	ln -sf $(INSTALL_LIB_DIR)$(TARGET) $(INSTALL_LIB_DIR)libspot.so 
ifneq ($(INSTALL_HEAD_DIR),$(INC_DIR))
	# create folder "libspot" for the headers
	mkdir -p $(INSTALL_HEAD_DIR)	
	# copy the headers to that folder
	cp -u $(DEPS) $(INSTALL_HEAD_DIR) 
	# move the shared library to the library folder			
	cp -u $(LIB_DIR)$(TARGET) $(INSTALL_LIB_DIR)
endif

# test spot on a gaussian white noise
testspot:
	@echo -- Building test -- 
	$(CC) $(CXXFLAGS) -I $(EXT_INC_DIR) -I $(INSTALL_HEAD_DIR) -L $(INSTALL_LIB_DIR) $(TEST_DIR)testspot.cpp -lspot -o $(TEST_DIR)testspot
	@echo
	@echo -- Testing --
	$(EXPORT); $(TEST_DIR)testspot


test: testspot 

# clean
clean:
	@rm -rfd $(OBJ_DIR)
	@rm -rfd $(LIB_DIR)
	@rm -rf $(TEST_DIR)testspot





