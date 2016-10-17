#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H

#include "vulkan/vulkan.h"

class VulkanUtils {
public:
	static const char* getVkResultString(int result);
	static const char* getVkResultString(VkResult result);
private:
};

#endif
