.PHONY: all test clean

CXX = g++
CXXFLAGS = -Wall -g
LDFLAGS = /usr/lib/librelic_s.a_bls12-381 -lgmp
EXE=test_policy.out

RELIC_INCLUDE=/usr/include/relic_bls12-381
LEGACY_OBJ = zobject.o zfunctioninput.o zattributelist.o zpolicy.o zdriver.o zscanner.o zparser.tab.o zgroup.o zlsss.o zelement_bp.o

all: $(EXE)

$(EXE):
	$(CXX) $(CXXFLAGS) -c legacy/zattributelist.cpp legacy/zfunctioninput.cpp legacy/zobject.cpp legacy/zpolicy.cpp
	$(CXX) $(CXXFLAGS) -c legacy/parser/zdriver.cpp legacy/parser/zscanner.cpp legacy/parser/zparser.tab.cc
	$(CXX) $(CXXFLAGS) -c legacy/arith/zgroup.cpp
	$(CXX) $(CXXFLAGS) -c legacy/arith/zelement_bp.cpp -I$(RELIC_INCLUDE)
	$(CXX) $(CXXFLAGS) -c legacy/lsss/zlsss.cpp -I$(RELIC_INCLUDE)
	$(CXX) -o $@ $(CXXFLAGS) $(LEGACY_OBJ) -I$(RELIC_INCLUDE) test_policy.cpp $(LDFLAGS)

test: $(EXE)
	./$(EXE) logic "A5 and ((A1 and A2) or (A3 and A4))" "A5|A1|A2"

clean:
	rm -f *.o *.out *~
