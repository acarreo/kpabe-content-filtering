.PHONY: all compile clean

OBJ_DIR = build
SRC = $(wildcard *.cpp)
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: compile

compile: $(OBJ_DIR) $(OBJ) $(OBJ_DIR)/zparser.tab.o

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

$(OBJ_DIR)/zparser.tab.o: zparser.tab.cc
	$(CC) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) *.o *~
