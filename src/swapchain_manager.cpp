#include "swapchain_manager.h"

SwapchainManager::SwapchainManager(VulkanState& vulkanState, Window& window):
	mVulkanState(vulkanState), 
	mWindow(window),
	mVulkanImageCreator(vulkanState),
	mDepthImageDesc(vulkanState.device)
{

}

SwapchainManager::~SwapchainManager() 
{

}

void SwapchainManager::createSurface()
{
	VK_CHECK_RESULT(glfwCreateWindowSurface(mVulkanState.instance, mWindow.mGlfwWindow, nullptr, &mVulkanState.surface));
	
	if (glfwVulkanSupported() == GLFW_FALSE)
		throw std::runtime_error("Vulkan is not supported by GLFW. Cannot create a surface");

}

void SwapchainManager::createSwapChain()
{
	VkSurfaceFormatKHR surfaceFormat = getSurfaceFormat(mVulkanState.swapChainDesc.surfaceFormats);
	VkPresentModeKHR presentMode = getPresentMode(mVulkanState.swapChainDesc.presentModes);
	VkExtent2D extent = getExtent(mVulkanState.swapChainDesc.surfaceCapabilities); 

	uint32_t numImages = mVulkanState.swapChainDesc.surfaceCapabilities.minImageCount;
	numImages = std::min(numImages + 1, mVulkanState.swapChainDesc.surfaceCapabilities.maxImageCount);
	
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mVulkanState.surface;
	createInfo.minImageCount = numImages;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	if (mVulkanState.graphicsQueueIndex != mVulkanState.presentQueueIndex) {
		uint32_t queueFamilyIndices[] = { 
			(uint32_t) mVulkanState.graphicsQueueIndex, 
			(uint32_t) mVulkanState.presentQueueIndex 
		};
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
		LOG("OTHER");
	} else {
		LOG("SAME");
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = mVulkanState.swapChainDesc.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	VkSwapchainKHR oldSwapChain = mVulkanState.swapChain;
	createInfo.oldSwapchain = oldSwapChain;
	VkSwapchainKHR swapChain;
	LOG("BEFORE SWAPCHAIN");
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mVulkanState.device, &createInfo, nullptr, &swapChain));
	LOG("AFTER SWAPCHAIN");
	mVulkanState.swapChain = swapChain;
	vkGetSwapchainImagesKHR(mVulkanState.device, mVulkanState.swapChain, &numImages, nullptr);
	mSwapChainImages.resize(numImages);
	vkGetSwapchainImagesKHR(mVulkanState.device, mVulkanState.swapChain, &numImages, mSwapChainImages.data());
	
	mVulkanState.swapChainImageFormat = surfaceFormat.format;
	mVulkanState.swapChainExtent = extent;

	LOG("SWAP CHAIN CREATED");
}

void SwapchainManager::createImageViews() 
{
	ImageHelper vic(mVulkanState);
	mSwapChainImageViews.resize(mSwapChainImages.size());
	for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
		vic.createImageView(mSwapChainImages[i], mVulkanState.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mSwapChainImageViews[i]);
		LOG("IMAGE VIEW CREATED");
	}
}

void SwapchainManager::createDepthResources() 
{
	VkFormat depthFormat = mVulkanImageCreator.findDepthFormat();

	mVulkanImageCreator.createImage(
			mVulkanState.swapChainExtent.width, 
			mVulkanState.swapChainExtent.height, 
			depthFormat, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mDepthImageDesc.image,
			mDepthImageDesc.memory);

	mVulkanImageCreator.createImageView(
			mDepthImageDesc.image, 
			depthFormat, 
			VK_IMAGE_ASPECT_DEPTH_BIT,
			mDepthImageDesc.imageView);

	mVulkanImageCreator.transitionImageLayout(
			mDepthImageDesc.image, 
			depthFormat, 
			VK_IMAGE_LAYOUT_UNDEFINED, 
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}



void SwapchainManager::createFramebuffers(VkRenderPass renderPass)
{
	mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = renderPass;
	createInfo.width = mVulkanState.swapChainExtent.width;
	createInfo.height = mVulkanState.swapChainExtent.height;
	createInfo.layers = 1;

	for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i) {
		std::array<VkImageView, 2> attachments = { 
			mSwapChainImageViews[i],
			mDepthImageDesc.imageView
		};

		createInfo.attachmentCount = attachments.size();
		createInfo.pAttachments = attachments.data();

		VK_CHECK_RESULT(vkCreateFramebuffer(mVulkanState.device, &createInfo, nullptr, &mSwapChainFramebuffers[i]));
		LOG("FRAMEBUFFER CREATED");
	}
}

VkSurfaceFormatKHR SwapchainManager::getSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const 
{
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for (const auto& surfaceFormat : surfaceFormats)
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return surfaceFormat;

	return surfaceFormats[0];
}

VkPresentModeKHR SwapchainManager::getPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const
{
	for (const auto& presentMode : presentModes)
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentMode;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::getExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const
{
	VkExtent2D extent;
	extent.width = std::max(
			surfaceCapabilities.minImageExtent.width, 
			std::min(mWindow.getWidth(), surfaceCapabilities.maxImageExtent.width));
	extent.height = std::max(
			surfaceCapabilities.minImageExtent.height, 
			std::min(mWindow.getHeight(), surfaceCapabilities.maxImageExtent.height));
	
	return extent;
}


SwapChainDesc SwapchainManager::getSwapChainDesc(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
	SwapChainDesc swapChainDesc = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainDesc.surfaceCapabilities);
	uint32_t numSurfaceFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSurfaceFormats, nullptr);
	
	if (numSurfaceFormats > 0) {
		swapChainDesc.surfaceFormats.resize(numSurfaceFormats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
				physicalDevice, 
				surface, 
				&numSurfaceFormats, 
				swapChainDesc.surfaceFormats.data());
	}

	uint32_t numPresentModes;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numPresentModes, nullptr);

	if (numPresentModes > 0) {
		swapChainDesc.presentModes.resize(numPresentModes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice, 
				surface, 
				&numPresentModes,
				swapChainDesc.presentModes.data());
	}

	return swapChainDesc;
}


void SwapchainManager::createCommandPool()
{
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = mVulkanState.graphicsQueueIndex; 
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK_RESULT(vkCreateCommandPool(mVulkanState.device, &createInfo, nullptr, &mVulkanState.commandPool));
	LOG("COMMAND BUFFER CREATED");
}

void SwapchainManager::createCommandBuffers()
{
	mVkCommandBuffers.resize(mSwapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mVulkanState.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) mVkCommandBuffers.size();
	
	VK_CHECK_RESULT(vkAllocateCommandBuffers(mVulkanState.device, &allocInfo, mVkCommandBuffers.data()));

	LOG("COMMAND POOL ALLOCATED");

//	updateCommandBuffers();
}

void SwapchainManager::createSemaphores()
{
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_CHECK_RESULT(vkCreateSemaphore(mVulkanState.device, &createInfo, nullptr, &mImageAvailableSemaphore));
	VK_CHECK_RESULT(vkCreateSemaphore(mVulkanState.device, &createInfo, nullptr, &mRenderFinishedSemaphore));
	LOG("SEMAPHORES CREATED");
}
