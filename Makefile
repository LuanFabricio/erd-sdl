BUILD_FOLDER := build
TARGET := $(BUILD_FOLDER)/main
CFLAGS = -Iinclude

SRC_DIR := src
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c')

all: build
	cc -o $(TARGET) ./main.c $(SRC_FILES) $(CFLAGS)

build_folder:
	mkdir -p $(BUILD_FOLDER)

run: all
	$(TARGET)
