# Compiler & Flags
# -O3: Maximum optimization (Critical for HFT benchmarking)
# -I./include: Looks for headers in your include folder
# -Wall -Wextra: Enable all standard warnings.
CC := gcc
CFLAGS := -std=c11 -O3 -Wall -Wextra -I./include

# Directories
SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests

# Targets
TARGET := bench_alloc
VERIFIER := verify_layout

# Source Files
SRCS := $(SRC_DIR)/slab.c $(SRC_DIR)/bench_alloc.c
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Phony Targets (Not actual files)
.PHONY: all clean test

# Default Build
all: $(TARGET)

# Link the Benchmark Binary
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build Complete. Run ./$(TARGET)"

# Compile Source to Objects
# The $| ensures the build directory exists before compiling
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build the Layout Verifier (Week 4 Requirement)
# Compiles tests/test_layout.c and links against slab.o (if needed)
$(VERIFIER): $(TEST_DIR)/test_layout.c
	@echo "Building Layout Verifier..."
	$(CC) $(CFLAGS) $< -o $@

# Create Build Directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(VERIFIER)
	./$(VERIFIER)

# Clean Workspace
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(TARGET) $(VERIFIER)
