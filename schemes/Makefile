
.PHONY: all clean

OBJ_DIR = build
SRC = kpabe.cpp test_kpabe.cpp
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: $(OBJ_DIR) $(OBJ)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I$(RELIC_INCLUDE)

clean:
	rm -rf $(OBJ_DIR) *.o *~
