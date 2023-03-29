.PHONY: all clean

LIB_LSSS = liblsss_bls12-381.a
OBJ_DIR = build
SRC = $(wildcard *.cpp)
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(OBJ_DIR) $(LIB_LSSS)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

$(OBJ_DIR)/zparser.tab.o: zparser.tab.cc
	$(CC) -o $@ -c $< $(CXXFLAGS)

$(LIB_LSSS): $(OBJ) $(OBJ_DIR)/zparser.tab.o
	ar rcs $@ $^

clean:
	rm -rf $(OBJ_DIR) *.o *~
