.PHONY: all clean

CXXFLAGS = -Wall -g
CXX = g++

RELIC_LIB_BLS_12_381=/usr/lib/librelic_s.a_bls12-381
RELIC_INCLUDE_BLS_12_381=/usr/include/relic_bls12-381

LEGACY_OBJ = zobject.o zfunctioninput.o zattributelist.o zpolicy.o zdriver.o zscanner.o zparser.tab.o zgroup.o zlsss.o zelement_bp.o

all: tests

run: tests
	./test_policy.out logic "A5 and ((A1 and A2) or (A3 and A4))" "A5|A1|A2"

tests:
	$(CXX) $(CXXFLAGS) -c legacy/zattributelist.cpp legacy/zfunctioninput.cpp legacy/zobject.cpp legacy/zpolicy.cpp
	$(CXX) $(CXXFLAGS) -c legacy/parser/zdriver.cpp legacy/parser/zscanner.cpp legacy/parser/zparser.tab.cc
	$(CXX) $(CXXFLAGS) -c legacy/arith/zgroup.cpp
	$(CXX) $(CXXFLAGS) -c legacy/arith/zelement_bp.cpp -I$(RELIC_INCLUDE_BLS_12_381)
	$(CXX) $(CXXFLAGS) -c legacy/lsss/zlsss.cpp -I$(RELIC_INCLUDE_BLS_12_381)
	$(CXX) -o test_policy.out $(CXXFLAGS) $(LEGACY_OBJ) -I$(RELIC_INCLUDE_BLS_12_381) test_policy.cpp $(RELIC_LIB_BLS_12_381) -lgmp 

clean:
	rm -f *.o *.out *~
