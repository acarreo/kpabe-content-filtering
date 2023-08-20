.PHONY: all clean

export CC = cc
export CXX = g++
export CCFLAGS = -Wall -g
export CXXFLAGS = $(CCFLAGS) -std=c++20
export RELIC_INCLUDE = /usr/local/include/relic
LDFLAGS = -lrelic_s -lgmp

DPVS_DIR = dpvs
DPVS_OBJ = $(wildcard $(DPVS_DIR)/build/*.o)
OBJ = $(DPVS_OBJ) utils.o serial.o test_serial.o

all: dpvs_compile test_serial.out

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

dpvs_compile:
	$(MAKE) -C $(DPVS_DIR)

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

serial.o: serial/serial.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

# test_serial.o: test_serial.c
# 	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

test_serial.out: dpvs_compile utils.o serial.o test_serial.o
	$(CC) -o $@ $(CCFLAGS) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(DPVS_DIR)/build *.o *~ *.out

vars:
	@echo $(OBJ)
