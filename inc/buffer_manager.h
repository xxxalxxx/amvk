#ifndef AMVK_BUFFER_MANAGER_H
#define AMVK_BUFFER_MANAGER_H

#include "vulkan/vulkan.h"

class BufferManager {
public:
	uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags);
	void createBuffer(VkBuffer& buffer, 
					  VkDeviceSize size, 
					  VkDeviceMemory& memory, 
					  VkMemoryPropertyFlags prop, 
					  VkBufferUsageFlags usage);

	void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize size);

};

#endif
