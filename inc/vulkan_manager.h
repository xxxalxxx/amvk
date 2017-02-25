#ifndef AMVK_VULKAN_MANAGER_H
#define AMVK_VULKAN_MANAGER_H

#define VK_THROW_RESULT_ERROR(text, result) \
	do { \
		char str[128]; \
		int resultCode = static_cast<int>(result); \
		sprintf(str, #text " VkResult: %s (code: %d)", VulkanUtils::getVkResultString(resultCode), resultCode); \
		throw std::runtime_error(str); \
	} while (0)

#define VK_CHECK_RESULT(f)  \
	do { \
		VkResult result = f; \
		if (result != VK_SUCCESS) \
			VK_THROW_RESULT_ERROR(f, result); \
	} while (0)

#define VK_CALL_IPROC(instance, func, ...) \
	do { \
		auto __##func = (PFN_##func) vkGetInstanceProcAddr(instance, #func); \
		if (!__##func) \
			throw std::runtime_error("Failed to get Vulkan instance procedure for " #func); \
		if (__##func(__VA_ARGS__) != VK_SUCCESS) \
		   throw std::runtime_error("Failed to call iproc for " #func); \
	} while (0)

#include <limits>
#include <cstring>
#include <vector>
#include <string>
#include <stdio.h>
#include <unordered_set>
#include <cstddef>

#include "macro.h"
#include "window.h"
#include "camera.h"
#include "timer.h"
#include "device_queue_indices.h"
#include "file_manager.h"
#include "vulkan_utils.h"
#include "vulkan_pipeline_creator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "texture_manager.h"
#include "quad.h"

class VulkanManager { 
	friend class Engine;
	struct SwapChainDesc;
public:

	VulkanManager(const Window& window);
	virtual ~VulkanManager();
	void init();
	void createVkInstance();
	void createVkSurface(GLFWwindow& glfwWindow);
	void enableDebug();
	void createPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain(const Window& window);
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();


	void createDepthResources();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
	

	void updateCommandBuffers(const Timer& timer, Camera& camera);
	void draw();
	
	void waitIdle();
	void recreateSwapChain();

	const VkDevice& getVkDevice() const;

	uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags);
	void createBuffer(VkBuffer& buffer, 
					  VkDeviceSize size, 
					  VkDeviceMemory& memory,  
					  VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags prop);

	void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
	
	std::vector<VkExtensionProperties> getVkExtensionProperties();
	std::vector<std::string> getVkExtensionPropertyNames(std::vector<VkExtensionProperties>& extensionProperties);
	std::vector<const char*> getExtensionNames();

private:
	void updateUniformBuffer(const Timer& timer);

	DeviceQueueIndicies getDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice);
	bool deviceExtensionsSupported(const VkPhysicalDevice& physicalDevice); 
	
	SwapChainDesc getSwapChainDesc(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	VkExtent2D getExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities, const Window& window);
	VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkSurfaceFormatKHR getSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
	


	DeviceQueueIndicies mDeviceQueueIndices;


	struct SwapChainDesc {
		VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> mSurfaceFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	SwapChainDesc mSwapChainDesc;
	//VkSwapchainKHR mVkSwapChain;
	std::vector<VkImage> mSwapChainImages;
	std::vector<VkImageView> mSwapChainImageViews;
	std::vector<VkFramebuffer> mSwapChainFramebuffers;
	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;



	std::vector<VkCommandBuffer> mVkCommandBuffers;

	VkSemaphore mImageAvailableSemaphore, mRenderFinishedSemaphore;
	VkDebugReportCallbackEXT mDebugReportCallback; 


	const Window& mWindow;
	
	static const std::vector<const char*> sDeviceExtensions;
	static const std::vector<const char*> sValidationLayers;

	void createImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuf);
	
	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMem;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler;




	size_t mNumIndices;
	VkBuffer vertexBuffer, indexBuffer, uniformBuffer, uniformStagingBuffer;
	VkDeviceMemory vertexBufferMem, indexBufferMem, uniformBufferMem, uniformStagingBufferMem;
	

	VkDescriptorSetLayout mVkDescriptorSetLayout;
	VkDescriptorPool mVkDescriptorPool;
	VkDescriptorSet mVkDescriptorSet;

	
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMem;
	VkImageView mDepthImageView;

	VulkanState mVulkanState;
	Quad mQuad;
};

#endif
