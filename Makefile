CC = g++
CFLAGS = -Wall -std=c++17 -I./include
FLEX = flex
BISON = bison
BISONFLAGS = -d -y -v

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Create directories if they don't exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Source files
LEXER_SRC = 1705069.l
PARSER_SRC = 1705069.y
FLEX_OUT = lex.yy.c
BISON_OUT = y.tab.c
BISON_HDR = y.tab.h

# Additional source files
CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
CPP_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SRCS))

# Object files
OBJS = $(OBJ_DIR)/y.o $(OBJ_DIR)/l.o $(CPP_OBJS)

# Build targets
.PHONY: all clean run test

all: $(BIN_DIR)/compiler

# Generate parser from Bison grammar
$(BISON_OUT) $(BISON_HDR): $(PARSER_SRC)
	$(BISON) $(BISONFLAGS) $<

# Generate lexer from Flex grammar
$(FLEX_OUT): $(LEXER_SRC)
	$(FLEX) $<

# Compile parser
$(OBJ_DIR)/y.o: $(BISON_OUT) $(BISON_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile lexer
$(OBJ_DIR)/l.o: $(FLEX_OUT)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile C++ sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

# Link everything together
$(BIN_DIR)/compiler: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

# Clean build artifacts
clean:
	rm -f $(FLEX_OUT) $(BISON_OUT) $(BISON_HDR) y.output
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -f code.asm optimized.asm log.txt

# Run with an input file
run: $(BIN_DIR)/compiler
	./$(BIN_DIR)/compiler input.c

# Test with all test cases
test: $(BIN_DIR)/compiler
	@echo "Running tests..."
	@for file in Test\ Cases/*.c; do \
		echo "Testing $$file"; \
		./$(BIN_DIR)/compiler "$$file"; \
	done 