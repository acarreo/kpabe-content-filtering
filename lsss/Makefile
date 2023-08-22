.PHONY: all install clean

CC = cc
CXX = g++
CXXFLAGS = -Wall
LIB_LSSS = liblsss.a
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
INSTALL_DIR = /usr/local

all: $(LIB_LSSS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

zparser.tab.o: zparser.tab.cc
	$(CC) -o $@ -c $< $(CXXFLAGS)

$(LIB_LSSS): $(OBJ) zparser.tab.o
	ar rcs $@ $^

install: $(LIB_LSSS)
	mkdir -p $(INSTALL_DIR)/include/lsss
	cp *.h $(INSTALL_DIR)/include/lsss/
	cp zparser.yy zparser.tab.hh zscanner.ll $(INSTALL_DIR)/include/lsss/
	cp $(LIB_LSSS) $(INSTALL_DIR)/lib

clean:
	rm -f *.o *~ $(LIB_LSSS)
