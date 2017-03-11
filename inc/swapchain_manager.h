#ifndef AMVK_SWAPCHAIN_MANAGER_H
#define AMVK_SWAPCHAIN_MANAGER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

#include "vulkan_state.h"
#include "vulkan_utils.h"
#include "vulkan_image_creator.h"
#include "window.h"
#include "swap_chain_desc.h"
#include "quad.h"

class SwapchainManager {
public:
	SwapchainManager(VulkanState& vulkanState, Window& window);
	~SwapchainManager();
	void createSurface();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createFramebuffers(VkRenderPass renderPass);
	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
	void createRenderPass();

	VkSurfaceFormatKHR getSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const; 
	VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
	VkExtent2D getExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
	SwapChainDesc getSwapChainDesc(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	SwapChainDesc swapChainDesc;

	std::vector<VkFramebuffer> mSwapChainFramebuffers;
	std::vector<VkCommandBuffer> mVkCommandBuffers;

	VkSemaphore mImageAvailableSemaphore, mRenderFinishedSemaphore;
private:
	VulkanState& mVulkanState;
	Window& mWindow;

	std::vector<VkImage> mSwapChainImages;
	std::vector<VkImageView> mSwapChainImageViews;
	ImageHelper mVulkanImageCreator;
	VulkanImageDesc mDepthImageDesc;

};

#endif
