.PHONY: all clean

export CC = cc
export CXX = g++
export CCFLAGS = -Wall -g
export CXXFLAGS = $(CCFLAGS) -std=c++20
export RELIC_INCLUDE = /usr/local/include/relic
LDFLAGS = -lrelic_s -lgmp -lmsgpackc

DPVS_DIR = ../dpvs
SRC = serialization.cpp test_serialization.cpp
DPVS_OBJ = $(wildcard $(DPVS_DIR)/build/*.o)
SER_OBJ = $(SRC:.cpp=.o)
OBJ = $(DPVS_OBJ) utils.o

all: dpvs_compile test_main.out

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

dpvs_compile:
	$(MAKE) -C $(DPVS_DIR)

utils.o: ../utils.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)
	ldconfig -N

test_dpvs.o: test_dpvs.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

test_serial.o: test_serial.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

test_serial.out: dpvs_compile utils.o test_serial.o
	$(CXX) -o $@ $(CCFLAGS) $(DPVS_OBJ) utils.o test_serial.o $(LDFLAGS)

test_main.out: dpvs_compile $(SER_OBJ) $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) utils.o $(SER_OBJ) ../dpvs/build/matrix.o ../dpvs/build/dpvs.o $(LDFLAGS)

clean:
	rm -rf $(DPVS_DIR)/build *.o *~ *.out

vars:
	@echo $(OBJ)
