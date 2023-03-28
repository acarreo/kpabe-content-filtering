
.PHONY: all compile clean

OBJ_DIR = build
SRC = matrix.c dpvs.c
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

all: compile

compile: $(OBJ_DIR) $(OBJ)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS) -I$(RELIC_INCLUDE)

clean:
	rm -rf $(OBJ_DIR) *.o *~
