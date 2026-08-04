BUILD_FOLDER := build
TARGET := $(BUILD_FOLDER)/main
CFLAGS = -Iinclude -lm -lraylib -ggdb

SRC_DIR := src
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')

all: build_folder
	cc -o $(TARGET) ./main.c $(SRC_FILES) $(CFLAGS)

build_folder:
	mkdir -p $(BUILD_FOLDER)

run: all
	$(TARGET)

debug: all
	gf2 ./$(TARGET)
