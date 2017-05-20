#ifndef AMVK_VULKAN_STATE_H
#define AMVK_VULKAN_STATE_H

#include <vulkan/vulkan.h>
#include "swap_chain_desc.h"


struct DeviceInfo {
	DeviceInfo():
	maxPushConstantsSize(0),
	minUniformBufferOffsetAlignment(0)
	{}
	uint32_t maxPushConstantsSize;
	VkDeviceSize minUniformBufferOffsetAlignment;
};

struct Pipelines {

};

struct DescriptorSets {

};

struct VulkanState {
	VulkanState(): 
		instance(VK_NULL_HANDLE), 
		physicalDevice(VK_NULL_HANDLE), 
		device(VK_NULL_HANDLE), 
		swapChain(VK_NULL_HANDLE),
		graphicsQueue(VK_NULL_HANDLE), 
		presentQueue(VK_NULL_HANDLE),
		commandPool(VK_NULL_HANDLE) 
	{};
	
	// Disallow copy constructor for VulkanState.
	// Only references for VulkanState are allowed
	VulkanState(VulkanState const& vulkanState) = delete;
    VulkanState& operator=(VulkanState const& vulkanState) = delete;
	
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	
	VkSwapchainKHR swapChain;
	SwapChainDesc swapChainDesc;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkQueue graphicsQueue;
	VkQueue presentQueue; 
	uint32_t graphicsQueueIndex;
	uint32_t presentQueueIndex;

	VkRenderPass renderPass;
	
	VkCommandPool commandPool;

	VkFormat depthFormat;

	DeviceInfo deviceInfo;
	Pipelines pipelines;
	DescriptorSets descriptorSets;
};

#endif
