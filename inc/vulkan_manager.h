#ifndef AMVK_VULKAN_MANAGER_H
#define AMVK_VULKAN_MANAGER_H

#include "vulkan/vulkan.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <unordered_set>
#include "macro.h"
#include "window.h"
#include "device_queue_indices.h"

class VulkanManager { 
	friend class Engine;
public:
	VulkanManager();
	virtual ~VulkanManager();
	void init();
	void createVkInstance();
	void createVkSurface(GLFWwindow& glfwWindow);
	void enableDebug();
	void createPhysicalDevice();

	void outDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice, DeviceQueueIndicies& deviceQueueIndicies); 
	void outDeviceExtensionsSupport(const VkPhysicalDevice& physicalDevice, bool& extenstionsSupported); 

	std::vector<VkExtensionProperties> getVkExtensionProperties();
	std::vector<std::string> getVkExtensionPropertyNames(std::vector<VkExtensionProperties>& extensionProperties);
	std::vector<const char*> getExtensionNames();
	static const char* sGetVkResultString(int result);
	static const char* sGetVkResultString(VkResult result);
private:
	VkInstance mVkInstance;
	VkSurfaceKHR mVkSurface;
	VkPhysicalDevice mVkPhysicalDevice;	
	
	DeviceQueueIndicies mDeviceQueueIndices;
	
	static const std::vector<const char*> deviceExtensions;
	static const std::vector<const char*> validationLayers;
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
