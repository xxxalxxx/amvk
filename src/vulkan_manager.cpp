#include "vulkan_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


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


const std::vector<const char*> VulkanManager::sDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> VulkanManager::sValidationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};




VulkanManager::VulkanManager(const Window& window):
	mVkPhysicalDevice(VK_NULL_HANDLE),
	mVkSwapChain(VK_NULL_HANDLE),
	mWindow(window),
	mNumIndices(0),
	vertexBuffer(VK_NULL_HANDLE),
	indexBuffer(VK_NULL_HANDLE),
	uniformBuffer(VK_NULL_HANDLE),
	uniformStagingBuffer(VK_NULL_HANDLE),
	vertexBufferMem(VK_NULL_HANDLE),
	indexBufferMem(VK_NULL_HANDLE),
	uniformBufferMem(VK_NULL_HANDLE),
	uniformStagingBufferMem(VK_NULL_HANDLE)
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
	unsigned numExt = 0;
	const char **ppExtenstions = glfwGetRequiredInstanceExtensions(&numExt);
	std::vector<const char*> extensions;
	extensions.reserve(numExt);
	for (unsigned i = 0; i < numExt; ++i)
		extensions.push_back(ppExtenstions[i]);

	#ifdef AMVK_DBG
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	#endif

	return extensions;
}

void VulkanManager::createVkInstance() 
{
	#ifdef AMVK_DBG
	{
		uint32_t numLayers;
		vkEnumerateInstanceExtensionProperties(&numLayers, nullptr);
		std::vector<VkLayerProperties> props(numLayers);
		vkEnumerateInstanceExtensionProperties(&numLayers, props.data());
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
	instanceInfo.enabledLayerCount = sValidationLayers.size();
	instanceInfo.ppEnabledLayerNames = sValidationLayers.data();
	#else
	instanceInfo.enabledLayerCount = 0;
	#endif

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
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
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
		DeviceQueueIndicies dqi = getDeviceQueueFamilyIndices(device);
		bool extenstionsSupported = deviceExtensionsSupported(device);
		
		if (!extenstionsSupported)
			continue;

		SwapChainDesc swapChainDesc = getSwapChainDesc(device, mVkSurface);
		bool swapChainSupported = !swapChainDesc.mSurfaceFormats.empty() && !swapChainDesc.mPresentModes.empty();

		if (!swapChainSupported) 
			continue;

		if (dqi.graphicsIndexSet() && dqi.supportedIndexSet()) {
			mVkPhysicalDevice = device;
			mDeviceQueueIndices = dqi;
			mSwapChainDesc = swapChainDesc;
			LOG("DEVICE INITIALIZED");
			break;
		}
	}

	if (mVkPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No valid physical device found");
}

void VulkanManager::createLogicalDevice() 
{
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::unordered_set<int> uniqueIndices;
	
	for (size_t i = 0; i < NUM_DEVICE_QUEUE_INDICES; ++i) {
		uniqueIndices.insert(mDeviceQueueIndices[i]);
	}

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
	
	#ifdef AMVK_DBG
	createInfo.enabledLayerCount = sValidationLayers.size();
	createInfo.ppEnabledLayerNames = sValidationLayers.data(); 
	#else
	createInfo.enabledLayerCount = 0;
	#endif
	
	VK_CHECK_RESULT(vkCreateDevice(mVkPhysicalDevice, &createInfo, nullptr, &mVkDevice));
	vkGetDeviceQueue(mVkDevice, mDeviceQueueIndices.getGraphicsQueueIndex(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mVkDevice, mDeviceQueueIndices.getSupportedQueueIndex(), 0, &mSupportedQueue);
	LOG("LOGICAL DEVICE CREATED");
}

DeviceQueueIndicies VulkanManager::getDeviceQueueFamilyIndices(const VkPhysicalDevice& physicalDevice)
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
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, mVkSurface, &surfaceSupported));

			if (surfaceSupported) 
				deviceQueueIndicies.setSupportedIndex(i);

			if (deviceQueueIndicies.graphicsIndexSet() && deviceQueueIndicies.supportedIndexSet())
				break;
		}
	}
	return deviceQueueIndicies;
}

bool VulkanManager::deviceExtensionsSupported(const VkPhysicalDevice& physicalDevice) 
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

void VulkanManager::createSwapChain(const Window& window)
{
	VkSurfaceFormatKHR surfaceFormat = getSurfaceFormat(mSwapChainDesc.mSurfaceFormats);
	VkPresentModeKHR presentMode = getPresentMode(mSwapChainDesc.mPresentModes);
	VkExtent2D extent = getExtent(mSwapChainDesc.mSurfaceCapabilities, window); 

	uint32_t numImages = mSwapChainDesc.mSurfaceCapabilities.minImageCount;
	numImages = std::min(numImages + 1, mSwapChainDesc.mSurfaceCapabilities.maxImageCount);
	
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mVkSurface;
	createInfo.minImageCount = numImages;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	if (mDeviceQueueIndices.getGraphicsQueueIndex() != mDeviceQueueIndices.getSupportedQueueIndex()) {
		uint32_t queueFamilyIndices[] = { 
			(uint32_t) mDeviceQueueIndices.getGraphicsQueueIndex(), 
			(uint32_t) mDeviceQueueIndices.getSupportedQueueIndex() 
		};
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = mSwapChainDesc.mSurfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR oldSwapChain = mVkSwapChain;

	createInfo.oldSwapchain = oldSwapChain;
	VkSwapchainKHR swapChain;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mVkDevice, &createInfo, nullptr, &swapChain));
	mVkSwapChain = swapChain;
	vkGetSwapchainImagesKHR(mVkDevice, mVkSwapChain, &numImages, nullptr);
	mSwapChainImages.resize(numImages);
	vkGetSwapchainImagesKHR(mVkDevice, mVkSwapChain, &numImages, mSwapChainImages.data());
	
	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;

	LOG("SWAP CHAIN CREATED");
}


VulkanManager::SwapChainDesc VulkanManager::getSwapChainDesc(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface)
{
	SwapChainDesc swapChainDesc = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainDesc.mSurfaceCapabilities);
	uint32_t numSurfaceFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSurfaceFormats, nullptr);
	
	if (numSurfaceFormats > 0) {
		swapChainDesc.mSurfaceFormats.resize(numSurfaceFormats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, 
											 surface, 
											 &numSurfaceFormats, 
											 swapChainDesc.mSurfaceFormats.data());
	}

	uint32_t numPresentModes;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numPresentModes, nullptr);

	if (numPresentModes > 0) {
		swapChainDesc.mPresentModes.resize(numPresentModes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
												  surface, 
												  &numPresentModes,
												  swapChainDesc.mPresentModes.data());
	}

	return swapChainDesc;
}

VkSurfaceFormatKHR VulkanManager::getSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) 
{
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for (const auto& surfaceFormat : surfaceFormats)
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return surfaceFormat;

	return surfaceFormats[0];
}

VkPresentModeKHR VulkanManager::getPresentMode(const std::vector<VkPresentModeKHR>& presentModes) 
{
	for (const auto& presentMode : presentModes)
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentMode;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanManager::getExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities, const Window& window)
{
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return surfaceCapabilities.currentExtent;
	} else {
		VkExtent2D extent;
		extent.width = std::max(surfaceCapabilities.minImageExtent.width, 
								std::min(window.getWidth(), surfaceCapabilities.maxImageExtent.width));
		extent.height = std::max(surfaceCapabilities.minImageExtent.height, 
								 std::min(window.getHeight(), surfaceCapabilities.maxImageExtent.height));
		return extent;
	}
}

void VulkanManager::createImageViews() 
{
	mSwapChainImageViews.resize(mSwapChainImages.size());
	for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
		createImageView(mSwapChainImages[i], mSwapChainImageFormat, mSwapChainImageViews[i]);
		LOG("IMAGE VIEW CREATED");
	}
}


void VulkanManager::createRenderPass()
{
	VkAttachmentDescription att = {};
	att.format = mSwapChainImageFormat;
	att.samples = VK_SAMPLE_COUNT_1_BIT;
	att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	att.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attRef = {};
	attRef.attachment = 0;
	attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription sub = {};
	sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	sub.colorAttachmentCount = 1;
	sub.pColorAttachments = &attRef;
	
	VkSubpassDependency dependancy = {};
	dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependancy.dstSubpass = 0;
	dependancy.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependancy.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &att;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &sub;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependancy;

	VK_CHECK_RESULT(vkCreateRenderPass(mVkDevice, &createInfo, nullptr, &mVkRenderPass));
	LOG("RENDER PASS CREATED");
}

void VulkanManager::createPipeline()
{
	createDescriptorSetLayout();



	FileManager& fileManager = FileManager::getInstance();
	auto vertShaderSrc = fileManager.readShader("shader.vert.spv");
	auto fragShaderSrc = fileManager.readShader("shader.frag.spv");


	createShaderModule(vertShaderSrc, vertShaderModule);
	createShaderModule(fragShaderSrc, fragShaderModule);

	VkPipelineShaderStageCreateInfo vertStageCreateInfo = {};
	vertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageCreateInfo.module = vertShaderModule;
	vertStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
	fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageCreateInfo.module = fragShaderModule;
	fragStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] = {
		vertStageCreateInfo,
		fragStageCreateInfo
	};

	auto bindingDesc = getBindingDesc();
	auto attrDesc = getAttrDesc();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = attrDesc.size();
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo = {};
	assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyInfo.primitiveRestartEnable = VK_FALSE;
	LOG("EXTENT W:" << mSwapChainExtent.width << " H:" << mSwapChainExtent.height);
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) mSwapChainExtent.width;
	viewport.height = (float) mSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = mSwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;	
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizationState = {};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationState.lineWidth = 1.0f;
	rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachmentState = {};
	blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
										| VK_COLOR_COMPONENT_G_BIT 
										| VK_COLOR_COMPONENT_B_BIT 
										| VK_COLOR_COMPONENT_A_BIT;
	blendAttachmentState.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo blendState = {};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.logicOpEnable = VK_FALSE;
	blendState.logicOp = VK_LOGIC_OP_COPY;
	blendState.attachmentCount = 1;
	blendState.pAttachments = &blendAttachmentState;
	blendState.blendConstants[0] = 0.0f;
	blendState.blendConstants[1] = 0.0f;
	blendState.blendConstants[2] = 0.0f;
	blendState.blendConstants[3] = 0.0f;

	VkDescriptorSetLayout setLayouts[] = { mVkDescriptorSetLayout };
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = setLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VK_CHECK_RESULT(vkCreatePipelineLayout(mVkDevice, &pipelineLayoutInfo, nullptr, &mVkPipelineLayout));

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &assemblyInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizationState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pColorBlendState = &blendState;
	pipelineInfo.layout = mVkPipelineLayout;
	pipelineInfo.renderPass = mVkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(mVkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline));
	LOG("PIPELINE CREATED");
}

void VulkanManager::createShaderModule(const std::vector<char>& shaderSpvCode, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderSpvCode.size();
	createInfo.pCode = (uint32_t*) shaderSpvCode.data();
	VK_CHECK_RESULT(vkCreateShaderModule(mVkDevice, &createInfo, nullptr, &shaderModule));
	LOG("SHADER MODULE CREATED sz:" << shaderSpvCode.size());
}


void VulkanManager::createFramebuffers()
{
	mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

	for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i) {
		VkImageView attachments[] = { mSwapChainImageViews[i] };
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = mVkRenderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.width = mSwapChainExtent.width;
		createInfo.height = mSwapChainExtent.height;
		createInfo.layers = 1;
		VK_CHECK_RESULT(vkCreateFramebuffer(mVkDevice, &createInfo, nullptr, &mSwapChainFramebuffers[i]));
		LOG("FRAMEBUFFER CREATED");
	}
}

void VulkanManager::createCommandPool()
{
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = mDeviceQueueIndices.getGraphicsQueueIndex();
	VK_CHECK_RESULT(vkCreateCommandPool(mVkDevice, &createInfo, nullptr, &mVkCommandPool));
	LOG("COMMAND BUFFER CREATED");
}

void VulkanManager::createCommandBuffers()
{
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();

	if (!mVkCommandBuffers.empty())
		vkFreeCommandBuffers(mVkDevice, mVkCommandPool, mVkCommandBuffers.size(), mVkCommandBuffers.data());

	mVkCommandBuffers.resize(mSwapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mVkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) mVkCommandBuffers.size();
	
	VK_CHECK_RESULT(vkAllocateCommandBuffers(mVkDevice, &allocInfo, mVkCommandBuffers.data()));

	LOG("COMMAND POOL ALLOCATED");

	for (size_t i = 0; i < mVkCommandBuffers.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		
		VK_CHECK_RESULT(vkBeginCommandBuffer(mVkCommandBuffers[i], &beginInfo));

		VkClearValue clearValues = {0.2f, 0.0f, 0.0f, 1.0f};

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = mVkRenderPass;
		renderPassBeginInfo.framebuffer = mSwapChainFramebuffers[i];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = mSwapChainExtent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValues;

		vkCmdBeginRenderPass(mVkCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mVkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mVkPipeline);
		VkBuffer vertBuf[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(mVkCommandBuffers[i], 0, 1, vertBuf, offsets);
		vkCmdBindIndexBuffer(mVkCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(mVkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mVkPipelineLayout, 0, 1, &mVkDescriptorSet, 0, nullptr);
		vkCmdDrawIndexed(mVkCommandBuffers[i], mNumIndices, 1, 0, 0, 0);

		vkCmdEndRenderPass(mVkCommandBuffers[i]);

		VK_CHECK_RESULT(vkEndCommandBuffer(mVkCommandBuffers[i]));
	}
}

void VulkanManager::createSemaphores()
{
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_CHECK_RESULT(vkCreateSemaphore(mVkDevice, &createInfo, nullptr, &mImageAvailableSemaphore));
	VK_CHECK_RESULT(vkCreateSemaphore(mVkDevice, &createInfo, nullptr, &mRenderFinishedSemaphore));
	LOG("SEMAPHORES CREATED");
}

void VulkanManager::draw() 
{

	uint32_t imageIndex = 0;

	VkResult result = vkAcquireNextImageKHR(mVkDevice, 
										  mVkSwapChain, 
										  std::numeric_limits<uint64_t>::max(), 
										  mImageAvailableSemaphore, 
										  VK_NULL_HANDLE, 
										  &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
	} else if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {	
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		
		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphore };
		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphore };
		VkSwapchainKHR swapChains[] = { mVkSwapChain };
		VkPipelineStageFlags stageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = stageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mVkCommandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK_RESULT(vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
		
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VK_CHECK_RESULT(vkQueuePresentKHR(mSupportedQueue, &presentInfo));

	} else {
		char str[128]; \
		int resultCode = static_cast<int>(result); \
		sprintf(str,"failed vkAcquireNextImageKHR Result: %s (code: %d)", 
				VulkanManager::sGetVkResultString(resultCode), 
				resultCode); 
		throw std::runtime_error(str);
	}
}

void VulkanManager::waitIdle() 
{
	vkDeviceWaitIdle(mVkDevice);
}

void VulkanManager::recreateSwapChain()
{
	waitIdle();
	
	createSwapChain(mWindow);
    createImageViews();
    createRenderPass();
    createPipeline();
    createFramebuffers();
    createCommandBuffers();
}


uint32_t VulkanManager::getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags)
{
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &memProps);

	for (size_t i = 0; i < memProps.memoryTypeCount; ++i)
		if ((typeFilter & (1 << i)) 
		&& (memProps.memoryTypes[i].propertyFlags & flags))
			return i;
	throw std::runtime_error("Failed to find memory type");
}


void VulkanManager::createBuffer(VkBuffer& buffer, 
				  VkDeviceSize size, 
				  VkDeviceMemory& memory, 
				  VkBufferUsageFlags usage,
				  VkMemoryPropertyFlags prop)
{
	VkBufferCreateInfo buffInfo = {};
	buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffInfo.size = size;
	buffInfo.usage = usage;
	buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (buffer != VK_NULL_HANDLE) {
	
	}
	LOG("a");	
	VK_CHECK_RESULT(vkCreateBuffer(mVkDevice, &buffInfo, nullptr, &buffer));
	
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(mVkDevice, buffer, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, prop);
	
	if (memory != VK_NULL_HANDLE) {
	
	}
	LOG("b");
	VK_CHECK_RESULT(vkAllocateMemory(mVkDevice, &allocInfo, nullptr, &memory));
	vkBindBufferMemory(mVkDevice, buffer, memory, 0);
	LOG("c");
}

void VulkanManager::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkCommandBuffer cmdBuf = beginSingleTimeCommands();

	VkBufferCopy bufferCopy = {};
	bufferCopy.size = size;

	vkCmdCopyBuffer(cmdBuf, src, dst, 1, &bufferCopy);

	endSingleTimeCommands(cmdBuf);
}


void VulkanManager::createVertexBuffer() 
{
	const std::vector<Vertex> vertices = {
	    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	VkDeviceSize bufSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuf;
	VkDeviceMemory stagingBufDeviceMem;
	createBuffer(stagingBuf, 
								bufSize, 
								stagingBufDeviceMem, 
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);	
	void* data;
	vkMapMemory(mVkDevice, stagingBufDeviceMem, 0, bufSize, 0 , &data);
	memcpy(data, vertices.data(), (size_t) bufSize);
	vkUnmapMemory(mVkDevice, stagingBufDeviceMem);
	createBuffer(vertexBuffer, 
								bufSize, 
								vertexBufferMem, 
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
								VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	LOG("4");
	copyBuffer(stagingBuf, vertexBuffer, bufSize);
}

void VulkanManager::createIndexBuffer()
{
	const std::vector<uint32_t> indices = {
	    0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
	
	mNumIndices = indices.size(); 

	LOG("num indices:" << mNumIndices);
	

	VkDeviceSize bufSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuf;
	VkDeviceMemory stagingBufDeviceMem;
	createBuffer(stagingBuf, 
								bufSize, 
								stagingBufDeviceMem, 
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);	
	void* data;
	vkMapMemory(mVkDevice, stagingBufDeviceMem, 0, bufSize, 0 , &data);
	memcpy(data, indices.data(), (size_t) bufSize);
	vkUnmapMemory(mVkDevice, stagingBufDeviceMem);
	createBuffer(indexBuffer, 
								bufSize, 
								indexBufferMem, 
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
								VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	copyBuffer(stagingBuf, indexBuffer, bufSize);
}


VkVertexInputBindingDescription VulkanManager::getBindingDesc()
{
	VkVertexInputBindingDescription bindDesc = {};
	bindDesc.binding = 0;
	bindDesc.stride = sizeof(Vertex);
	bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindDesc;
}

std::array<VkVertexInputAttributeDescription, 3> VulkanManager::getAttrDesc() 
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);


	return attributeDescriptions;
}

	
const VkDevice& VulkanManager::getVkDevice() const 
{
	return mVkDevice;
}
	

void VulkanManager::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descSetBinding = {};
	descSetBinding.binding = 0;
	descSetBinding.descriptorCount = 1;
	descSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descSetBinding.pImmutableSamplers = nullptr;
	descSetBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {descSetBinding, samplerLayoutBinding};

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = bindings.size();
	descSetLayoutInfo.pBindings = bindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVkDevice, &descSetLayoutInfo, nullptr, &mVkDescriptorSetLayout));
	LOG("DESC LAYOUT CREATED");
}

void VulkanManager::createUniformBuffer()
{
	VkDeviceSize bufSize = sizeof(UBO);
	createBuffer(uniformStagingBuffer, 
				bufSize, 
				uniformStagingBufferMem, 
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	createBuffer(uniformBuffer, 
				bufSize, 
				uniformBufferMem, 
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
}

void VulkanManager::createDescriptorSet() 
{
	VkDescriptorSetLayout layouts[] = { mVkDescriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = mVkDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(mVkDevice, &allocInfo, &mVkDescriptorSet));

	VkDescriptorBufferInfo buffInfo = {};
	buffInfo.buffer = uniformBuffer;
	buffInfo.offset = 0;
	buffInfo.range = sizeof(UBO);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mTextureImageView;
	imageInfo.sampler = mTextureSampler;

	std::array<VkWriteDescriptorSet, 2> writeSets = {};

	writeSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSets[0].dstSet = mVkDescriptorSet;
	writeSets[0].dstBinding = 0;
	writeSets[0].dstArrayElement = 0;
	writeSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSets[0].descriptorCount = 1;
	writeSets[0].pBufferInfo = &buffInfo;

	writeSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSets[1].dstSet = mVkDescriptorSet;
	writeSets[1].dstBinding = 1;
	writeSets[1].dstArrayElement = 0;
	writeSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeSets[1].descriptorCount = 1;
	writeSets[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(mVkDevice, writeSets.size(), writeSets.data(), 0, nullptr);
}

void VulkanManager::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	VK_CHECK_RESULT(vkCreateDescriptorPool(mVkDevice, &poolInfo, nullptr, &mVkDescriptorPool));
}

void VulkanManager::updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	UBO ubo = {};
	ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float) mSwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	
	void* data;
//	LOG("m1 w:" << mSwapChainExtent.width << " h:" << mSwapChainExtent.height);
	vkMapMemory(mVkDevice, uniformStagingBufferMem, 0, sizeof(ubo), 0, &data);
//	LOG("m2");
	memcpy(data, &ubo, sizeof(ubo));
//	LOG("m3");
	vkUnmapMemory(mVkDevice, uniformStagingBufferMem);
//	LOG("m4");
	copyBuffer(uniformStagingBuffer, uniformBuffer, sizeof(ubo));	
}

void VulkanManager::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer cmdBuf = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	 if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED 
			 && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED 
			&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
			&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(cmdBuf,
						VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						0, 
						0, nullptr, 
						0, nullptr, 
						1, &barrier);

	endSingleTimeCommands(cmdBuf);
}

void VulkanManager::copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height)
{
	VkCommandBuffer cmdBuf = beginSingleTimeCommands();

	VkImageSubresourceLayers subRes = {};
	subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRes.baseArrayLayer = 0;
	subRes.mipLevel = 0;
	subRes.layerCount = 1;

	VkImageCopy copy = {};
	copy.srcSubresource = subRes;
	copy.dstSubresource = subRes;
	copy.srcOffset = {0, 0, 0};
	copy.dstOffset = {0, 0, 0};
	copy.extent.width = width;
	copy.extent.height = height;
	copy.extent.depth = 1;

	vkCmdCopyImage(cmdBuf, 
				   srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				   dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				   1, &copy);

	endSingleTimeCommands(cmdBuf);
}

void VulkanManager::createImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling,
				VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = w;
	imageInfo.extent.height = h;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_RESULT(vkCreateImage(mVkDevice, &imageInfo, nullptr, &image));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(mVkDevice, image, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, properties);

	VK_CHECK_RESULT(vkAllocateMemory(mVkDevice, &allocInfo, nullptr, &imageMemory));

	vkBindImageMemory(mVkDevice, image, imageMemory, 0);
}

void VulkanManager::createTextureImage()
{
	FileManager& fm = FileManager::getInstance();
	std::string path = fm.getResourcePath("texture/statue.jpg");
	LOG("PATH:" << path);
	int w, h, channels;
	stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, STBI_rgb_alpha);

	VkDeviceSize imageSize = w * h * 4;

	if (!pixels)
		throw std::runtime_error("Unable to load image");

	VkImage stagingImg;
	VkDeviceMemory stagingImageMem;
	LOG("z");
	createImage(w, h, 
				VK_FORMAT_R8G8B8A8_UNORM, 
				VK_IMAGE_TILING_LINEAR,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				stagingImg, stagingImageMem);
	LOG("zz");
	void* data;
	vkMapMemory(mVkDevice, stagingImageMem, 0, imageSize, 0, &data);
	memcpy(data, pixels, (size_t) imageSize);
	vkUnmapMemory(mVkDevice, stagingImageMem);

	stbi_image_free(pixels);

    createImage(w, h, 
				VK_FORMAT_R8G8B8A8_UNORM, 
				VK_IMAGE_TILING_OPTIMAL, 
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
				mTextureImage, mTextureImageMem);

	transitionImageLayout(stagingImg,
						VK_IMAGE_LAYOUT_PREINITIALIZED, 
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	transitionImageLayout(mTextureImage, 
			VK_IMAGE_LAYOUT_PREINITIALIZED, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	
	copyImage(stagingImg, mTextureImage, w, h);

	transitionImageLayout(mTextureImage, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void VulkanManager::createImageView(VkImage image, VkFormat format, VkImageView& imageView)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK_RESULT(vkCreateImageView(mVkDevice, &viewInfo, nullptr, &imageView));
}

void VulkanManager::createTextureImageView()
{
	createImageView(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, mTextureImageView);
}

void VulkanManager::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	VK_CHECK_RESULT(vkCreateSampler(mVkDevice, &samplerInfo, nullptr, &mTextureSampler));
}

VkCommandBuffer VulkanManager::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mVkCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuf;
	vkAllocateCommandBuffers(mVkDevice, &allocInfo, &cmdBuf);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuf, &beginInfo);
	
	return cmdBuf;
}

void VulkanManager::endSingleTimeCommands(VkCommandBuffer cmdBuf)
{
	vkEndCommandBuffer(cmdBuf);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType =  VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuf;

	vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mGraphicsQueue);
	vkFreeCommandBuffers(mVkDevice, mVkCommandPool, 1, &cmdBuf);
}

