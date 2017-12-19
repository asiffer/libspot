# makefile libstream
INC_DIR = ../../include/
SRC_DIR = ../../src/
OBJ_DIR = build/

CC = g++
CXXFLAGS = -std=c++14 -Wall -pedantic -I/usr/include/python3.5m/ -I $(INC_DIR)



FILES = ubend.h streammoments.h streamstats.h
DEPS = $(foreach n,$(FILES),$(INC_DIR)$(n))
SRCS = $($(foreach n,$(FILES),$(SRC_DIR)$(n)):.h=.cpp)
OBJS = $(FILES:.h=.o) libstream_wrap.o

all: _libstream.so


# Shared libraries
_libstream.so: $(OBJS)
	g++ -std=c++14 -shared $^ -o $@ -fPIC
	mv *.o *.cxx $(OBJ_DIR)
	
%.o: $(SRC_DIR)%.cpp
	$(CC) $(CXXFLAGS) -c $^ -fPIC

# Wrapper .o files
libstream_wrap.o: libstream_wrap.cxx
	$(CC) $(CXXFLAGS) -c libstream_wrap.cxx -fPIC


# Wrapper .cxx files
libstream_wrap.cxx: libstream.i
	swig -I$(INC_DIR) -c++ -python libstream.i


clean:
	rm $(OBJ_DIR)*.o *.so $(OBJ_DIR)*wrap.cxx libstream.py
