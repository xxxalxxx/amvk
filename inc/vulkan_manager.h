#ifndef AMVK_VULKAN_MANAGER_H
#define AMVK_VULKAN_MANAGER_H

#define GLM_FORCE_RADIANS


#include <limits>
#include <cstring>
#include <vector>
#include <string>
#include <stdio.h>
#include <unordered_set>
#include <cstddef>

#include "macro.h"
#include "window.h"
#include "device_queue_indices.h"
#include "file_manager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



#include <chrono>

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
	void createPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();
	
	void updateUniformBuffer();
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
	
	static const char* sGetVkResultString(int result);
	static const char* sGetVkResultString(VkResult result);

private:
	DeviceQueueIndicies getDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice);
	bool deviceExtensionsSupported(const VkPhysicalDevice& physicalDevice); 
	
	SwapChainDesc getSwapChainDesc(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	VkExtent2D getExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities, const Window& window);
	VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkSurfaceFormatKHR getSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
	
	void createShaderModule(const std::vector<char>& shaderSpvCode, VkShaderModule& shaderModule);

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
	VkDebugReportCallbackEXT msgCallback; 

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	
	const Window& mWindow;
	
	static constexpr const char* SHADER_ENTRY_NAME = "main";
	static const std::vector<const char*> sDeviceExtensions;
	static const std::vector<const char*> sValidationLayers;


	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	struct UBO {
	    glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	void createImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height);
	void createImageView(VkImage image, VkFormat format, VkImageView& imageView);
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuf);
	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMem;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler;



	void createVertexBuffer();
	void createIndexBuffer();

	VkVertexInputBindingDescription getBindingDesc();
	std::array<VkVertexInputAttributeDescription, 3> getAttrDesc();

	VkBuffer vertexBuffer, indexBuffer, uniformBuffer, uniformStagingBuffer;
	VkDeviceMemory vertexBufferMem, indexBufferMem, uniformBufferMem, uniformStagingBufferMem;
	

	void createDescriptorSetLayout();
	void createUniformBuffer();
	void createDescriptorPool();
	void createDescriptorSet();

	VkDescriptorSetLayout mVkDescriptorSetLayout;
	VkDescriptorPool mVkDescriptorPool;
	VkDescriptorSet mVkDescriptorSet;
};


#define VK_CHECK_RESULT(f) { \
	VkResult result = (f); \
	if (result != VK_SUCCESS) { \
		char str[128]; \
		int resultCode = static_cast<int>(result); \
		sprintf(str, #f " VkResult: %s (code: %d)", VulkanManager::sGetVkResultString(resultCode), resultCode); \
		throw std::runtime_error(str); \
	} \
}

#define VK_CALL_IPROC(instance, func, ...) \
		do { \
			auto __##func = (PFN_##func) vkGetInstanceProcAddr(instance, #func); \
			if (!__##func) { \
				throw std::runtime_error("Failed to get Vulkan instance procedure for " #func); \
			} \
			if (__##func(__VA_ARGS__) != VK_SUCCESS) { \
			   throw std::runtime_error("Failed to call iproc for " #func); \
			} \
		} while(0)

/*
#define VK_GET_IPROC(NAME) \
	do { \
		*(void **)&swapChain->fp##NAME = swapChainGetInstanceProc(swapChain, "vk" #NAME); \
		if (!swapChain->fp##NAME) { \
				fprintf(stderr, "Failed to get Vulkan instance procedure: `%s'\n", #NAME); \
				return false; \
				} \
	} while (0) 

#define VK_GET_DPROC(NAME) \
	do { \
		*(void **)&swapChain->fp##NAME = swapChainGetDeviceProc(swapChain, "vk" #NAME); \
			if (!swapChain->fp##NAME) { \
				printf(stderr, "Failed to get Vulkan device procedure: `%s'\n", #NAME); \
				return false; \
			} \
	} while (0)
*/
#endif
