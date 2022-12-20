
.PHONY: all clean

CXXFLAGS = -g -Wall -Wunused-function
CXX = gcc

RELIC_LIB=/usr/lib/librelic_s.a
RELIC_INCLUDE=/usr/include/relic

all: test

test:
	$(CXX) $(CXXFLAGS) -I$(RELIC_INCLUDE) -c matrix.c inv_mat.c dpvs.c test.c test_main.c
	$(CXX) -o test.out $(CXXFLAGS) *.o $(RELIC_LIB) -lgmp

clean:
	rm -f *.o test.out *~
