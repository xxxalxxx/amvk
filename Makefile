VULKAN_SDK_DIR := /home/al/VulkanSDK/1.0.21.1/x86_64
VULKAN_LIB_PATH := $(VULKAN_SDK_DIR)/lib
BIN_DIR := bin
INC_DIR := inc
SRC_DIR := src
OBJ_DIR := obj
SRC_DIRS := src

TARGET=$(BIN_DIR)/myengine
SRCS= $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
SRCS_NO_PATH = $(notdir $(SRCS))
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS_NO_PATH))
DEPS=$(patsubst %.cpp,$(OBJ_DIR)/%.d, $(SRCS_NO_PATH))

VPATH=inc:src:lib

CXX=g++
CXXFLAGS=-Wall -Werror -std=c++14 -I. -Isrc -Iinc -Ilib
RM=rm -f
LDLIBS=$(shell pkg-config --static --libs glfw3) -L$(VULKAN_LIB_PATH) -lvulkan

all: $(TARGET)
	LD_LIBRARY_PATH=$(VULKAN_LIB_PATH) VK_LAYER_PATH=$(VULKAN_SDK_DIR)/etc/explicit_layer.d ./$(TARGET)

$(TARGET): $(OBJS)
	@echo DEPS $(DEPS)
	@echo AT $@
	@echo DOLLAR $^
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	@echo ARROW $<
	@echo AT $@
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $< $(LDLIBS) 

.PHONY : clean

clean:
	$(RM) $(OBJ_DIR)/* $(TARGET)

-include $(DEPS)
