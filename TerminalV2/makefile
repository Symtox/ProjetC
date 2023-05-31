.PHONY: all clean

# Define default options
# One of PLATFORM_DESKTOP, PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
PROJECT_NAME       ?= game

# Define default C compiler: gcc
# NOTE: define g++ compiler if using C++
CC = gcc

# Define a recursive wildcard function
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Define all source files required
SRC_DIR = src
ENTITIES_DIR = entities
MOVEMENTS_DIR = movements
UTILS_DIR = utils
BIN_DIR = obj

#OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS ?= $(SRC_DIR)/$(ENTITIES_DIR)/attackAndDefense.c $(SRC_DIR)/$(MOVEMENTS_DIR)/moves.c $(SRC_DIR)/main.c $(SRC_DIR)/$(UTILS_DIR)/sleep.c $(SRC_DIR)/$(UTILS_DIR)/fileReader.c

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(PROJECT_NAME) $(OBJS) 

# Compile source files
# NOTE: This pattern will compile every module defined on $(OBJS)
#%.o: %.c
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@
