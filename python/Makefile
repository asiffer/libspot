# makefile libspot
INC_DIR = ../include/
SRC_DIR = ../src/
OBJ_DIR = build/

CC = g++
CXXFLAGS = -std=c++14 -Wall -pedantic -I/usr/include/python3.5m/ -I $(INC_DIR)



FILES = $(shell ls $(INC_DIR))#streamstats.h ubend.h brent.h gpdfit.h bounds.h spot.h dspot.h 
DEPS = $(foreach n,$(FILES),$(INC_DIR)$(n))
SRCS = $($(foreach n,$(FILES),$(SRC_DIR)$(n)):.h=.cpp)
OBJS = $(FILES:.h=.o) libspot_wrap.o

all: _libspot.so


# Shared libraries
_libspot.so: $(OBJS)
	g++ -std=c++14 -shared $^ -o $@ -fPIC
	mv *.o *.cxx $(OBJ_DIR)
	
%.o: $(SRC_DIR)%.cpp
	$(CC) $(CXXFLAGS) -c $^ -fPIC

# Wrapper .o files
libspot_wrap.o: libspot_wrap.cxx
	$(CC) $(CXXFLAGS) -c libspot_wrap.cxx -fPIC


# Wrapper .cxx files
libspot_wrap.cxx: libspot.i
	swig -I$(INC_DIR) -c++ -python libspot.i


clean:
	rm $(OBJ_DIR)*.o *.so $(OBJ_DIR)*wrap.cxx libspot.py
