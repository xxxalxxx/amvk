#ifndef AMVK_MACROS_H

#define AMVK_MACROS_H

#include <iostream>
#include <exception>
#include <string>
#include "vulkan/vulkan.h"

#define AMVK_DEBUG

#define GLM_FORCE_RADIANS
#define GLM_FORCE_ZERO_TO_ONE
#define GLM_VULKAN_PERSPECTIVE

#define LOG(x) do { std::cout << x << std::endl; } while(0)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


#endif


