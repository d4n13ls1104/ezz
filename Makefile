CC := gcc
CFLAGS := -std=gnu89 -Wall -Wextra

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

INCS := -I$(INCLUDE_DIR)

TARGET := ezz 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
