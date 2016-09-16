#ifndef AMVK_MACROS_H

#define AMVK_MACROS_H

#define AMVK_DBG

#include <iostream>
#include <exception>
#include <string>
#include "vulkan/vulkan.h"

#define NS(ns, name) ns ## _ ## name
#define LOG(x) do { std::cout << x << std::endl; } while(0)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


#endif


