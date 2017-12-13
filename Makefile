### libspot makefile
#
# @author = asr

### INSTALL DIRECTORIES
DESTDIR =
###


VERSION = 1.0

# Current directory
CURDIR = $(realpath .)

# folder of the headers
INC_DIR = $(CURDIR)/include

# folder of the sources
SRC_DIR = $(CURDIR)/src

#folder of the built sources
OBJ_DIR = $(CURDIR)/build

#folder of the shared library
LIB_DIR = $(CURDIR)/lib

#folder for the testspot
TEST_DIR = $(CURDIR)/test


###
INSTALL_HEAD_DIR = $(DESTDIR)/usr/include/libspot
INSTALL_LIB_DIR = $(DESTDIR)/usr/lib
###

EXPORT = @export LD_LIBRARY_PATH=$(LIB_DIR)

# compiler & flags
CC = @g++-5
CXXFLAGS = -std=c++14 -Wall -pedantic 


# all the files (header, sources, build)
FILES = streammoments.h streamstats.h bounds.h ubend.h brent.h gpdfit.h spot.h dspot.h
DEPS = $(foreach n,$(FILES),$(INC_DIR)/$(n))
SRCS = $(foreach n,$(FILES:.h=.cpp),$(SRC_DIR)/$(n))
OBJS = $(FILES:.h=.o)


# library file
TARGET = libspot.so


### MAKEFILE TARGETS
all: checkdir $(TARGET)

# create lib/ and build/ directories
checkdir:
	@echo
	@echo "==== libspot-dev" $(VERSION) "===="
	@echo
	@echo "Checking the library directory ("$(LIB_DIR)")"
	@mkdir -p $(LIB_DIR)
	@echo "Checking the object directory ("$(OBJ_DIR)")"
	@mkdir -p $(OBJ_DIR)
	@echo
	@echo "[Building sources]"

# create the shared library
$(TARGET): $(OBJS)
	@echo
	@echo "[Building library]"
	@echo "Building" $@ "..."
	$(CC) $(CXXFLAGS) -shared $(foreach n,$^,$(OBJ_DIR)/$(n)) -o $(LIB_DIR)/$@ -fPIC;
	@echo "[done]"

# build source files
%.o: $(SRC_DIR)/%.cpp
	@echo "Building" $@ "..."
	$(CC) $(CXXFLAGS) $(EXT_INC_DIR) -I $(INC_DIR) -c $< -o $(OBJ_DIR)/$@ -fPIC

## INSTALL
install:
	@echo "Checking the headers installation directory ("$(INSTALL_HEAD_DIR)")"
	@mkdir -p $(INSTALL_HEAD_DIR)
	@echo "Checking the library installation directory ("$(INSTALL_LIB_DIR)")"
	@mkdir -p $(INSTALL_LIB_DIR)
	
	@echo "Installing the shared library ("$(TARGET)")"
	@install -t $(INSTALL_LIB_DIR) $(LIB_DIR)/*.so
	@echo "Installing the headers"
	@install -t $(INSTALL_HEAD_DIR) $(INC_DIR)/*.h
	@echo "[done]"

## TEST
# test spot on a gaussian white noise
test_spot:
	@echo
	@echo "Building test ..."
	$(CC) $(CXXFLAGS) -I$(INC_DIR) -L$(LIB_DIR) -o $(TEST_DIR)/test_spot $(TEST_DIR)/test_spot.cpp -lspot 
	@echo "Running test ..."
	$(EXPORT); $(TEST_DIR)/test_spot
	
test_dspot:
	@echo
	@echo "Building test ..."
	$(CC) $(CXXFLAGS) -I$(INC_DIR) -L$(LIB_DIR) -o $(TEST_DIR)/test_dspot $(TEST_DIR)/test_dspot.cpp -lspot 
	@echo "Running test ..."
	$(EXPORT); $(TEST_DIR)/test_dspot


test: test_spot test_dspot

## CLEAN
clean:
	@rm -rfd $(OBJ_DIR)
	@rm -rfd $(LIB_DIR)
	@rm -rf $(TEST_DIR)/test_spot





