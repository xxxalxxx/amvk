#ifndef AMVK_VULKAN_STATE_H
#define AMVK_VULKAN_STATE_H

#include <vulkan/vulkan.h>
#include "swap_chain_desc.h"

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
};

#endif
