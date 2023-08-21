.PHONY: all clean

export CC = cc
export CXX = g++
export CCFLAGS = -Wall
export CXXFLAGS = $(CCFLAGS) -std=c++20
export RELIC_INCLUDE = /usr/local/include/relic
LDFLAGS = -Llsss -llsss -lrelic_s -lgmp

SRC_C = dpvs/matrix.c dpvs/dpvs.c dpvs/serial.c utils.c
SRC_CPP = dpvs/dpvs_advanced.cpp keys/keys.cpp kpabe/kpabe.cpp examples/examples.cpp

OBJ = $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o)
DEP = $(OBJ:.o=.d)

EXEC = example.out

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

-include $(DEP)

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)
	@ $(CC) -MM -MP -MT $@ -MF $(@:.o=.d) $< $(CCFLAGS) -I$(RELIC_INCLUDE)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)
	@ $(CXX) -MM -MP -MT $@ -MF $(@:.o=.d) $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

clean:
	rm -f $(EXEC) $(OBJ) $(DEP) *~
