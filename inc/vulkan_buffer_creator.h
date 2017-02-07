#ifndef AMVK_BUFFER_MANAGER_H
#define AMVK_BUFFER_MANAGER_H

#include "vulkan/vulkan.h"
#include "cmd_pass.h"
#include "vulkan_utils.h"
#include "vulkan_state.h"

class VulkanBufferDesc {
public:
	VulkanBufferDesc(const VkDevice& device);
	~VulkanBufferDesc();
	VkBuffer buffer;
	VkDeviceMemory memory;
private:
	const VkDevice& mVkDevice;
};

class VulkanBufferCreator {
public:
	VulkanBufferCreator(const VulkanState& vulkanState);
	~VulkanBufferCreator();
	
	uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags) const;
	void createBuffer(
			VkBuffer& buffer, 
			VkDeviceSize size, 
			VkDeviceMemory& memory,  
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags prop) const;

	void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) const;
private:
	const VulkanState& mVulkanState;
};

#endif
