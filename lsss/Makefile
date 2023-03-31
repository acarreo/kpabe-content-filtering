.PHONY: all clean

CC = cc
CXX = g++
CXXFLAGS = -Wall

LIB_LSSS = liblsss.a
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(LIB_LSSS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

zparser.tab.o: zparser.tab.cc
	$(CC) -o $@ -c $< $(CXXFLAGS)

$(LIB_LSSS): $(OBJ) zparser.tab.o
	ar rcs $@ $^

clean:
	rm -f *.o *~
