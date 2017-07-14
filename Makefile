VULKAN_SDK_DIR := /home/al/VulkanSDK/1.0.39.1/x86_64
VULKAN_LIB_PATH := $(VULKAN_SDK_DIR)/lib
BIN_DIR := bin
INC_DIR := inc
SRC_DIR := src
OBJ_DIR := obj
SHADER_SPV_DIR := shader/spv
CACHE_DIR := cache
SRC_DIRS := src src/shape

TARGET=$(BIN_DIR)/myengine
SRCS= $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
SRCS_NO_PATH = $(notdir $(SRCS))
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS_NO_PATH))
DEPS=$(patsubst %.cpp,$(OBJ_DIR)/%.d, $(SRCS_NO_PATH))

VPATH=\
	inc:\
	inc/shape:\
	inc/creator:\
	src:\
	src/shape:\
	lib

CXX=g++

CXXFLAGS= \
	-Wall \
	-g \
	-std=c++14 \
	-I. \
	-Iinc \
	-Iinc/creator \
	-Iinc/shape \
	-Isrc \
	-Isrc/shape \
	-Ilib

RM=rm -f
LDLIBS=$(shell pkg-config --static --libs glfw3) -L$(VULKAN_LIB_PATH) -lvulkan -lassimp

all: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJS)
	@echo DEPS $(DEPS)
	@echo @ $@
	@echo \^ $^
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	@echo \< $<
	@echo @ $@
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $< $(LDLIBS) 

.PHONY : clean

clean:
	$(RM) $(OBJ_DIR)/* $(TARGET) $(SHADER_SPV_DIR)/* $(CACHE_DIR)/*

-include $(DEPS)
