.PHONY: all compile_dpvs compile_schemes clean

CURVE ?= bls12-381

export CC = cc
export CXX = g++
export CCFLAGS = -Wall
export CXXFLAGS = $(CCFLAGS) -std=c++20
export RELIC_INCLUDE = /usr/include/relic_$(CURVE)
LDFLAGS = -lrelic_$(CURVE) -llsss_$(CURVE) -lgmp

EXEC = test_abe_$(CURVE)
DPVS_DIR = dpvs
SCHEMES_DIR = schemes
LSSS_DIR = lsss
DPVS_OBJ = $(wildcard $(DPVS_DIR)/build/*.o)
SCHEMES_OBJ = $(wildcard $(SCHEMES_DIR)/build/*.o)
OBJ = $(DPVS_OBJ) $(SCHEMES_OBJ) utils.o

all: $(EXEC)

test:
	@./test.sh $(CURVE)

compile_dpvs:
	$(MAKE) -C $(DPVS_DIR)

compile_schemes:
	$(MAKE) -C $(SCHEMES_DIR)

utils.o: utils.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

test_cunit.o: test_cunit.cpp
	$(CXX) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

$(EXEC): compile_dpvs compile_schemes utils.o
	@$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS)


test_cunit: compile_dpvs compile_schemes utils.o test_cunit.o
	@$(CXX) -o $@ $(CXXFLAGS) $(DPVS_OBJ) $(SCHEMES_DIR)/build/kpabe.o utils.o test_cunit.o $(LDFLAGS)

clean:
	rm -rf $(DPVS_DIR)/build $(SCHEMES_DIR)/build $(EXEC) *.o *~
