#include "vulkan_manager.h"

VkDebugReportCallbackEXT msgCallback; 

const char* VulkanManager::sGetVkResultString(int result) {
		switch(result) {
		case 0:
				return "VK_SUCCESS";
		case 1: 
				return "VK_NOT_READY";
		case 2: 
				return "VK_TIMEOUT";
		case 3: 
				return "VK_EVENT_SET";
		case 4: 
				return "VK_EVENT_RESET";
		case 5: 
				return "VK_INCOMPLETE";
		case -1: 
				return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case -2: 
				return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case -3: 
				return "VK_ERROR_INITIALIZATION_FAILED";
		case -4: 
				return "VK_ERROR_DEVICE_LOST";
		case -5: 
				return "VK_ERROR_MEMORY_MAP_FAILED";
		case -6: 
				return "VK_ERROR_LAYER_NOT_PRESET";
		case -7: 
				return "VK_ERROR_EXTENSTION_NOT_PRESENT";
		case -8: 
				return "VK_ERROR_FEATURE_NOT_PRESENT";
		case -9: 
				return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case -10: 
				return "VK_ERROR_TOO_MANY_OBJECTS";
		case -11: 
				return "VK_ERROR_FORMAT_IS_NOT_SUPPORTED";
		case -12:
				return "VK_ERROR_FRAGMENT_POOL";
		case -1000000000: 
				return "VK_ERROR_SURFACE_LOST_KHR";
		case -1000000001: 
				return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case -1000001003: 
				return "VK_ERROR_SUBOPTIMAL_KHR";
		case -1000001004: 
				return "VK_ERROR_OUT_OF_DATE_KHR";
		case -1000003001: 
				return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case -1000011001: 
				return "VK_ERROR_VALIDATION_FAILED_EXT";
		case -1000012000: 
				return "VK_ERROR_INVALID_SHADER_NV";
		default: 
				return "ERROR_CODE_NOT_FOUND";
		}
}

const char* VulkanManager::sGetVkResultString(VkResult result) {
	return sGetVkResultString(static_cast<int>(result));
}


const std::vector<const char*> VulkanManager::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> VulkanManager::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};




VulkanManager::VulkanManager():
	mVkPhysicalDevice(VK_NULL_HANDLE)
{

}

VulkanManager::~VulkanManager()
{

}

void VulkanManager::init() 
{

}

std::vector<VkExtensionProperties> VulkanManager::getVkExtensionProperties() 
{
	uint32_t numExt;
	vkEnumerateInstanceExtensionProperties(nullptr, &numExt, nullptr);
	std::vector<VkExtensionProperties> extensionProperties(numExt);
	vkEnumerateInstanceExtensionProperties(nullptr, &numExt, extensionProperties.data());
	LOG("N:" << extensionProperties.size());
	return extensionProperties;
}

std::vector<std::string> VulkanManager::getVkExtensionPropertyNames(std::vector<VkExtensionProperties>& vkExtensionProperties)
{
	std::vector<std::string> names;
	names.reserve(vkExtensionProperties.size());
	for (VkExtensionProperties prop : vkExtensionProperties) 
		names.push_back(std::string(prop.extensionName));
	return names;
}

std::vector<const char*> VulkanManager::getExtensionNames() 
{
	unsigned numExt;
	const char **ppExtenstions = glfwGetRequiredInstanceExtensions(&numExt);
	std::vector<const char*> extensions;
	extensions.reserve(numExt);
	for (unsigned i = 0; i < numExt; ++i)
		extensions.push_back(ppExtenstions[i]);

	#if defined(AMVK_DBG)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	#endif

	return extensions;
}

void VulkanManager::createVkInstance() 
{
	VkApplicationInfo applicationInfo;
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = "Learning Vulkan";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.pEngineName = "AMVK";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> extensionNames = getExtensionNames();
	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = nullptr;
	instanceInfo.enabledExtensionCount = extensionNames.size();
	instanceInfo.ppEnabledExtensionNames = extensionNames.data();

	VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &mVkInstance));
}

void VulkanManager::createVkSurface(GLFWwindow& glfwWindow)
{	
	VK_CHECK_RESULT(glfwCreateWindowSurface(mVkInstance, &glfwWindow, nullptr, &mVkSurface));
	
	if (glfwVulkanSupported() == GLFW_FALSE)
		throw std::runtime_error("Vulkan is not supported by GLFW. Cannot create surface");
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags, 
		VkDebugReportObjectTypeEXT objType, 
		uint64_t obj, 
		size_t location, 
		int32_t code, 
		const char* layerPrefix, 
		const char* msg, 
		void* userData) 
{
	std::cout << "validation layer: " << msg << std::endl;
	return VK_FALSE;
}


void VulkanManager::enableDebug() 
{
	VkDebugReportCallbackCreateInfoEXT createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	VK_CALL_IPROC(mVkInstance, vkCreateDebugReportCallbackEXT, mVkInstance, &createInfo, nullptr, &msgCallback);
}

void VulkanManager::createPhysicalDevice() 
{
	uint32_t numDevices = 0;
	vkEnumeratePhysicalDevices(mVkInstance, &numDevices, nullptr);

	if (!numDevices)
		throw std::runtime_error("No Physical devices found");

	std::vector<VkPhysicalDevice> devices(numDevices);
	vkEnumeratePhysicalDevices(mVkInstance, &numDevices, devices.data());

	for (const auto& device : devices) {
		DeviceQueueIndicies dqi;
		outDeviceQueueFamilyIndices(device, dqi);
		bool extenstionsSupported;
		outDeviceExtensionsSupport(device, extenstionsSupported);

		//TODO: add swap chain logic
		if (extenstionsSupported && dqi.graphicsIndexSet() && dqi.supportedIndexSet()) {
			mVkPhysicalDevice = device;
			LOG("DEVICE INITIALIZED");
			break;
		}
	}

	if (mVkPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No valid physical device found");
}

void VulkanManager::outDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice, DeviceQueueIndicies& deviceQueueIndicies) 
{	
	uint32_t numPhysicalDeviceQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, nullptr);
	std::vector<VkQueueFamilyProperties> properties(numPhysicalDeviceQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, properties.data());

	for (size_t i = 0; i < numPhysicalDeviceQueueFamilies; ++i) {
		VkQueueFamilyProperties& prop = properties[i];
		if (prop.queueCount > 0) {
			if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				deviceQueueIndicies.setGraphicsIndex(i);
			
			if (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) 
				deviceQueueIndicies.setComputeIndex(i);
			
			if (prop.queueFlags & VK_QUEUE_TRANSFER_BIT) 
				deviceQueueIndicies.setTransferIndex(i);

			VkBool32 surfaceSupported;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, mVkSurface, &surfaceSupported));

			if (surfaceSupported) 
				deviceQueueIndicies.setSupportedIndex(i);

			if (deviceQueueIndicies.allIndicesSet())
				break;
		}
	}
}

void VulkanManager::outDeviceExtensionsSupport(const VkPhysicalDevice& physicalDevice, bool& extenstionsSupported) 
{
	uint32_t numExt = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExt, nullptr);
	std::vector<VkExtensionProperties> props(numExt);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExt, props.data());
	std::unordered_set<std::string> propSet(deviceExtensions.begin(), deviceExtensions.end());
	
	for (const auto& prop : props) {
		propSet.erase(prop.extensionName);
		if (propSet.empty()) break;
	}

	extenstionsSupported = propSet.empty();
}

