.PHONY: all compile_dpvs compile_access_structure clean

export CC = cc
export CXX = g++
export CCFLAGS = -Wall
export CXXFLAGS = $(CCFLAGS) -std=c++20
export LDFLAGS = /usr/lib/librelic_s.a_bls12-381 -lgmp
export RELIC_INCLUDE = /usr/include/relic_bls12-381

EXEC = test_abe
DPVS_DIR = dpvs
SCHEMES = schemes
ACCESS_STRUCTURE_DIR = access_structure
OBJ_SCHEMES = schemes/kpabe.o schemes/test_lsss.o
OBJ = $(wildcard $(DPVS_DIR)/build/*.o) $(wildcard $(ACCESS_STRUCTURE_DIR)/build/*.o) $(OBJ_SCHEMES) utils.o

all: $(EXEC)

compile_dpvs:
	cd $(DPVS_DIR) && $(MAKE)

compile_access_structure:
	cd $(ACCESS_STRUCTURE_DIR) && $(MAKE)

schemes/%.o: schemes/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

utils.o: utils.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

$(EXEC): compile_dpvs compile_access_structure $(OBJ_SCHEMES) utils.o
	@$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(DPVS_DIR)/build $(ACCESS_STRUCTURE_DIR)/build schemes/*.o *.o *~
