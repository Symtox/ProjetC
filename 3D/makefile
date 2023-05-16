# C Compilation Configuration
TARGET_EXEC ?= app.exe


CC=gcc
CFLAGS=-Wall -g -lm -lraylib -lGL -lm -L lib/

BUILD_DIR ?= build
SRC_DIRS ?= src

SRCS := $(shell powershell "(Get-ChildItem $(subst /,\,$(SRC_DIRS)) -Recurse -Include *.c | Select-Object -ExpandProperty FullName -split \ProjetC\)[1]")
OBJS := $(subst \src\,\build\,$(SRCS))
OBJS := $(OBJS:.c=.o)


$(BUILD_DIR)\$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

# c source
$(OBJS): $(SRCS)
	echo $(SRCS)
	powershell "if (!(Test-Path '$(dir $@)')) { mkdir '$(dir $@)' }"
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	shell powershell if not exist $(dir $@) mkdir $(dir $@)
	$(RM) -r $(BUILD_DIR)
