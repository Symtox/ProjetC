SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

# C Compilation Configuration
TARGET_EXEC ?= game.out

CC=gcc
CFLAGS=-Wall -Wextra -g -std=c99 -lm -lncursesw

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

.RECIPEPREFIX = >

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
> $(CC) $(OBJS) -o $@ $(CFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
> $(MKDIR_P) $(dir $@)
> $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
> $(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p