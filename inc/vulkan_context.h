#ifndef AMVK_VULKAN_CONTEXT_H
#define AMVK_VULKAN_CONTEXT_H

#include <vector>
#include "device_queue_indices.h"
#include "vulkan/vulkan.h"

class VulkanContext {
public:
	VkInstance mVkInstance;
	VkSurfaceKHR mVkSurface;
	VkPhysicalDevice mVkPhysicalDevice;	
	VkDevice mVkDevice;
	DeviceQueueIndicies mDeviceQueueIndices;
	VkQueue mGraphicsQueue, mSupportedQueue;

	struct SwapChainDesc {
		VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> mSurfaceFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	SwapChainDesc mSwapChainDesc;
	VkSwapchainKHR mVkSwapChain;
	std::vector<VkImage> mSwapChainImages;
	std::vector<VkImageView> mSwapChainImageViews;
	std::vector<VkFramebuffer> mSwapChainFramebuffers;
	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;

	VkRenderPass mVkRenderPass;
	VkPipelineLayout mVkPipelineLayout;
	VkPipeline mVkPipeline;

	VkCommandPool mVkCommandPool;
	std::vector<VkCommandBuffer> mVkCommandBuffers;

	VkSemaphore mImageAvailableSemaphore, mRenderFinishedSemaphore;

};

#endif
