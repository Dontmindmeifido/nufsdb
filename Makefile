CXX = g++
CXXFLAGS = -g -O3 -Wall -Wextra -MMD -MP \
           -I./external/imgui -I./external/imgui/backends \
           -I./src/Crud -I./src/Database -I./src/Interface -I./src/Interpreter -I./src/Misc
           
LIBS = -lglfw -lGL

BUILD_DIR = build
TARGET = $(BUILD_DIR)/db_engine

USER_SRCS = $(wildcard src/*/*.cpp) \
            $(wildcard src/*.cpp)

IMGUI_SRCS = external/imgui/imgui.cpp \
             external/imgui/imgui_draw.cpp \
             external/imgui/imgui_tables.cpp \
             external/imgui/imgui_widgets.cpp \
             external/imgui/backends/imgui_impl_glfw.cpp \
             external/imgui/backends/imgui_impl_opengl3.cpp \
             external/imgui/misc/cpp/imgui_stdlib.cpp

SRCS = $(USER_SRCS) $(IMGUI_SRCS)

OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "Linking final executable: $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "Build complete! Run with ./$(TARGET)"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Nuking the build directory and binary..."
	rm -rf $(BUILD_DIR)

.PHONY: all clean

-include $(DEPS)