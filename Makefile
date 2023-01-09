.PHONY: all compile_dpvs compile_access_structure clean

export CC = cc
export CXX = g++
export CXXFLAGS = -Wall -g
export LDFLAGS = /usr/lib/librelic_s.a_bls12-381 -lgmp
export RELIC_INCLUDE = /usr/include/relic_bls12-381

EXEC = test_policy
DPVS_DIR = dpvs
SCHEMES = schemes
ACCESS_STRUCTURE_DIR = access_structure
OBJ_SCHEMES = schemes/init.o schemes/kpabe.o schemes/test_lsss.o
OBJ = $(wildcard $(DPVS_DIR)/build/*.o) $(wildcard $(ACCESS_STRUCTURE_DIR)/build/*.o) $(OBJ_SCHEMES)

all: $(EXEC)

compile_dpvs:
	cd $(DPVS_DIR) && $(MAKE)

compile_access_structure:
	cd $(ACCESS_STRUCTURE_DIR) && $(MAKE)

schemes/%.o: schemes/%.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

schemes/init.o: schemes/init.c
	$(CC) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

$(EXEC): compile_dpvs compile_access_structure $(OBJ_SCHEMES)
	@$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(DPVS_DIR)/build $(ACCESS_STRUCTURE_DIR)/build *~
