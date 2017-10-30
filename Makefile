SOURCE_DIR = src
BUILD_DIR = build

CC = gcc
CFLAGS = -O3 -pipe -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments
MAIN_FILES = cola_mensajes.c fuente.c procesador.c

C_SOURCES := $(filter-out $(MAIN_FILES), \
													$(shell find $(SOURCE_DIR) -name '*.c' | cut -d '/' -f 2-))
C_OBJECTS := $(foreach file, $(C_SOURCES), $(BUILD_DIR)/$(basename $(file)).o)

C_BINARIES := $(foreach file, $(MAIN_FILES), $(BUILD_DIR)/$(basename $(file)))

.PHONY: default all clean

default: $(C_BINARIES)

all: $(C_BINARIES)

$(C_OBJECTS): $(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Compiling object $@
	$(CC) -c $< $(CFLAGS) -o $@

$(C_BINARIES): $(BUILD_DIR)/%: $(SOURCE_DIR)/%.c $(C_OBJECTS)
	@mkdir -p $(dir $@)
	@echo Compiling binary $@
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR)
