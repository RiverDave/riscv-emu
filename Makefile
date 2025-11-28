CC := cc
CFLAGS := -std=c11 -Wall -Wextra
DEBUG_CFLAGS := -g -O0
RELEASE_CFLAGS := -O2


CC := cc
CFLAGS := -std=c11 -Wall -Wextra
DEBUG_CFLAGS := -g -O0
RELEASE_CFLAGS := -O2

SRCS := emu.c
BUILD_DIR := build
TARGET := $(BUILD_DIR)/emu

.PHONY: all clean run debug build-emu

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(SRCS)
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -o $@ $(SRCS)

build-emu: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $(TARGET) $(SRCS)

debug: build-emu
	# Start LLDB in the workspace terminal (interactive)
	lldb $(TARGET)

run: all
	$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
