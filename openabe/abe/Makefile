.PHONY: all clean

export CC = cc
export CXX = g++
export CXXFLAGS = -Wall -g
export LDFLAGS = /usr/lib/librelic_s.a -lgmp
export RELIC_INCLUDE = /usr/include/relic

EXEC = test.out
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC)

compile: $(OBJ)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE) -Iinclude

$(EXEC): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o
