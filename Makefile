# ============================================
# Shadow Engine 2D - Makefile
# ============================================

CXX      = C:/msys64/ucrt64/bin/g++.exe
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -IC:/msys64/ucrt64/include/SDL2 -IC:/msys64/ucrt64/include
LDFLAGS  = -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

# Directories
SRC_DIR   = src
BUILD_DIR = build
TARGET    = game2d.exe

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# Default target
all: dirs $(TARGET)
	@echo "[BUILD] Done! Run with: ./$(TARGET)"

dirs:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(TARGET) del $(TARGET)

run: all
	./$(TARGET)

debug: CXXFLAGS += -g -DDEBUG
debug: all

.PHONY: all clean run debug dirs
