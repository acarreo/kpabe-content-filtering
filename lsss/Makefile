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
	sudo mkdir -p $(INSTALL_DIR)/include/lsss
	sudo cp *.h zparser.yy zparser.tab.hh zscanner.ll $(INSTALL_DIR)/include/lsss/
	sudo mv $(LIB_LSSS) $(INSTALL_DIR)/lib

clean:
	rm -f *.o *~
