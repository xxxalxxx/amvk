VULKAN_SDK_DIR := /home/al/VulkanSDK/1.0.21.1/x86_64
VULKAN_LIB_PATH := $(VULKAN_SDK_DIR)/lib
BIN_DIR := bin
INC_DIR := inc
SRC_DIR := src
OBJ_DIR := obj

TARGET=$(BIN_DIR)/myengine
SRCS=main.cpp task_manager.cpp window.cpp engine.cpp input_manager.cpp vulkan_manager.cpp device_queue_indices.cpp file_manager.cpp
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS))
VPATH=inc:src:lib

CXX=g++
CXXFLAGS=-Wall -Werror -std=c++14 -I. -Isrc -Iinc -Ilib
RM=rm -f
LDLIBS=$(shell pkg-config --static --libs glfw3) -L$(VULKAN_LIB_PATH) -lvulkan

all: clean $(TARGET)
	LD_LIBRARY_PATH=$(VULKAN_LIB_PATH) VK_LAYER_PATH=$(VULKAN_SDK_DIR)/etc/explicit_layer.d ./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDLIBS) 

.PHONY : clean

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
