
.PHONY: all clean

CXXFLAGS = -g -Wall -Wunused-function
CXX = gcc

RELIC_LIB=/usr/lib/librelic_s.a_bls12-381
RELIC_INCLUDE=/usr/include/relic_bls12-381

all: test

test:
	$(CXX) $(CXXFLAGS) -I$(RELIC_INCLUDE) -c matrix.c inv_mat.c dpvs.c test.c test_main.c
	$(CXX) -o test.out $(CXXFLAGS) *.o $(RELIC_LIB) -lgmp

clean:
	rm -f *.o test.out *~
