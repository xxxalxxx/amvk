#include "device_manager.h"

const std::vector<const char*> DeviceManager::sDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> DeviceManager::sValidationLayers = {
	"VK_LAYER_LUNARG_standard_validation",
};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCb(
		VkDebugReportFlagsEXT flags, 
		VkDebugReportObjectTypeEXT objType, 
		uint64_t obj, 
		size_t location, 
		int32_t code, 
		const char* layerPrefix, 
		const char* msg, 
		void* userData) 
{
	std::cerr << ">>> VK_VALIDATION_LAYER: " << msg << std::endl;
	return VK_FALSE;
}

DeviceManager::DeviceManager(VulkanState& vulkanState): 
	mVulkanState(vulkanState)
{

}

void DeviceManager::createVkInstance() 
{
#ifdef AMVK_DEBUG
	{
		uint32_t numLayers;
		vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
		std::vector<VkLayerProperties> props(numLayers);
		vkEnumerateInstanceLayerProperties(&numLayers, props.data());
	
		for (const char* layerName : sValidationLayers) {
            bool layerFound = false;
			for (const auto& layerProperties : props) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
                }
			}

			if (!layerFound) {
				std::string msg = "Unable to found layer:";
				msg += layerName;
				throw std::runtime_error(msg);
			}
		}
	}
#endif

	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	applicationInfo.pApplicationName = "Learning Vulkan";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.pEngineName = "AMVK";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensionNames = getExtensionNames();
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.enabledExtensionCount = extensionNames.size();
	instanceInfo.ppEnabledExtensionNames = extensionNames.data();

#ifdef AMVK_DEBUG
	LOG("DEBUG EXISTS");
	instanceInfo.enabledLayerCount = sValidationLayers.size();
	instanceInfo.ppEnabledLayerNames = sValidationLayers.data();
#else
	LOG("DEBUG DOES NOT EXIST");
	instanceInfo.enabledLayerCount = 0;
#endif
	VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &mVulkanState.instance));
	LOG("INSTANCE CREATED");
}

void DeviceManager::enableDebug() 
{
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pNext = nullptr;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT 
					 | VK_DEBUG_REPORT_WARNING_BIT_EXT
					 | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	createInfo.pfnCallback = &debugCb;
	createInfo.pUserData = nullptr;

	VK_CALL_IPROC(mVulkanState.instance, vkCreateDebugReportCallbackEXT, mVulkanState.instance, &createInfo, nullptr, &mDebugReportCallback);
	LOG("DEBUG ENABLED");
}

void DeviceManager::createPhysicalDevice(SwapchainManager& swapchainManager) 
{
	uint32_t numDevices = 0;
	vkEnumeratePhysicalDevices(mVulkanState.instance, &numDevices, nullptr);

	if (!numDevices)
		throw std::runtime_error("No Physical devices found");

	std::vector<VkPhysicalDevice> devices(numDevices);
	vkEnumeratePhysicalDevices(mVulkanState.instance, &numDevices, devices.data());

	for (const auto& device : devices) {
		DeviceQueueIndicies dqi = getDeviceQueueFamilyIndices(device);
		bool extenstionsSupported = deviceExtensionsSupported(device);
		
		if (!extenstionsSupported)
			continue;

		SwapChainDesc swapChainDesc = swapchainManager.getSwapChainDesc(device, mVulkanState.surface);
		if (!swapChainDesc.supported()) 
			continue;

		if (dqi.graphicsIndexSet() && dqi.supportedIndexSet()) {
			mVulkanState.physicalDevice = device;
			mVulkanState.graphicsQueueIndex = dqi.getGraphicsQueueIndex();
			mVulkanState.presentQueueIndex = dqi.getSupportedQueueIndex();
			mVulkanState.swapChainDesc = swapChainDesc;
			mDeviceQueueIndices = dqi;
			LOG("DEVICE INITIALIZED");
			break;
		}
	}

	if (mVulkanState.physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No valid physical device found");
}



void DeviceManager::createLogicalDevice() 
{
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::unordered_set<int> uniqueIndices;
	
	for (size_t i = 0; i < 2; ++i) 
		uniqueIndices.insert(mDeviceQueueIndices[i]);

	for (int index : uniqueIndices) {
		LOG("DEVICE INDEX:" << index);
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = index;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	//TODO: be aware that extra femilies may mess stuff up
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = sDeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = sDeviceExtensions.data();
	
#ifdef AMVK_DEBUG
	createInfo.enabledLayerCount = sValidationLayers.size();
	createInfo.ppEnabledLayerNames = sValidationLayers.data(); 
#else
	createInfo.enabledLayerCount = 0;
#endif	
	VK_CHECK_RESULT(vkCreateDevice(mVulkanState.physicalDevice, &createInfo, nullptr, &mVulkanState.device));
	vkGetDeviceQueue(mVulkanState.device, mDeviceQueueIndices.getGraphicsQueueIndex(), 0, &mVulkanState.graphicsQueue);
	vkGetDeviceQueue(mVulkanState.device, mDeviceQueueIndices.getSupportedQueueIndex(), 0, &mVulkanState.presentQueue);
	LOG("LOGICAL DEVICE CREATED");


	//init device info
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	VkPhysicalDeviceProperties physicalDeviceProperties;

	vkGetPhysicalDeviceFeatures(mVulkanState.physicalDevice, &physicalDeviceFeatures);
	vkGetPhysicalDeviceProperties(mVulkanState.physicalDevice, &physicalDeviceProperties); 
	
	mVulkanState.deviceInfo.samplerAnisotropy = physicalDeviceFeatures.samplerAnisotropy;
	mVulkanState.deviceInfo.maxPushConstantsSize = physicalDeviceProperties.limits.maxPushConstantsSize;
	mVulkanState.deviceInfo.minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
	LOG("ANISOTROPY " << physicalDeviceFeatures.samplerAnisotropy); 
	LOG("MAX PUSH CONST SIZE max:" << mVulkanState.deviceInfo.maxPushConstantsSize);

	LOG("LOGICAL DEVICE CREATED");
}


std::vector<const char*> DeviceManager::getExtensionNames() 
{
	unsigned numExt = 0;
	const char **ppExtenstions = glfwGetRequiredInstanceExtensions(&numExt);
	std::vector<const char*> extensions;
	extensions.reserve(numExt);
	for (unsigned i = 0; i < numExt; ++i)
		extensions.push_back(ppExtenstions[i]);

	#ifdef AMVK_DEBUG
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	#endif

	return extensions;
}

DeviceQueueIndicies DeviceManager::getDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice) const
{
	DeviceQueueIndicies deviceQueueIndicies;
	uint32_t numPhysicalDeviceQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, nullptr);
	std::vector<VkQueueFamilyProperties> properties(numPhysicalDeviceQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, properties.data());

	for (size_t i = 0; i < numPhysicalDeviceQueueFamilies; ++i) {
		VkQueueFamilyProperties& prop = properties[i];
		if (prop.queueCount > 0) {
			if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				deviceQueueIndicies.setGraphicsIndex(i);
			
			//if (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) 
			//	deviceQueueIndicies.setComputeIndex(i);
			
			//if (prop.queueFlags & VK_QUEUE_TRANSFER_BIT) 
			//	deviceQueueIndicies.setTransferIndex(i);

			VkBool32 surfaceSupported = false;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, mVulkanState.surface, &surfaceSupported));

			if (surfaceSupported) 
				deviceQueueIndicies.setSupportedIndex(i);

			if (deviceQueueIndicies.graphicsIndexSet() && deviceQueueIndicies.supportedIndexSet())
				break;
		}
	}
	return deviceQueueIndicies;
}

bool DeviceManager::deviceExtensionsSupported(const VkPhysicalDevice& physicalDevice) const 
{
	uint32_t numExt = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExt, nullptr);
	std::vector<VkExtensionProperties> props(numExt);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExt, props.data());
	std::unordered_set<std::string> propSet(sDeviceExtensions.begin(), sDeviceExtensions.end());
	
	for (const auto& prop : props) {
		propSet.erase(prop.extensionName);
		if (propSet.empty()) 
			return true;
	}

	return propSet.empty();
}

