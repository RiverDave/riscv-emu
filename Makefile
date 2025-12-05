CC := cc
CFLAGS := -std=c11 -Wall -Wextra
DEBUG_CFLAGS := -g -O0
RELEASE_CFLAGS := -O2

SRCS := emu.c
TEST_SRCS := tests.c
BUILD_DIR := build
TARGET := $(BUILD_DIR)/emu
TEST_TARGET := $(BUILD_DIR)/tests
EMU_OBJ := $(BUILD_DIR)/emu_core.o

.PHONY: all clean run debug build-emu test

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile emu core (without main) as object file for linking with tests
$(EMU_OBJ): $(BUILD_DIR) $(SRCS) emu.h
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -DEMU_NO_MAIN -c -o $@ $(SRCS)

$(TARGET): $(BUILD_DIR) $(SRCS) emu.h
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -o $@ $(SRCS)

build-emu: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $(TARGET) $(SRCS)

debug: build-emu
	# Start LLDB in the workspace terminal (interactive)
	lldb $(TARGET)

run: all
	$(TARGET)

test: $(EMU_OBJ)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS) $(EMU_OBJ)
	$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)
