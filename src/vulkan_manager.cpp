#include "vulkan_manager.h"



const std::vector<const char*> VulkanManager::sDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> VulkanManager::sValidationLayers = {
	"VK_LAYER_LUNARG_standard_validation",
};

VulkanManager::VulkanManager(const Window& window):
	//mVulkanState.physicalDevice(VK_NULL_HANDLE),
	//mVulkanState.swapChain(VK_NULL_HANDLE),
	mWindow(window),
	mNumIndices(0),
	vertexBuffer(VK_NULL_HANDLE),
	indexBuffer(VK_NULL_HANDLE),
	uniformBuffer(VK_NULL_HANDLE),
	uniformStagingBuffer(VK_NULL_HANDLE),
	vertexBufferMem(VK_NULL_HANDLE),
	indexBufferMem(VK_NULL_HANDLE),
	uniformBufferMem(VK_NULL_HANDLE),
	uniformStagingBufferMem(VK_NULL_HANDLE),
	mDepthImage(VK_NULL_HANDLE),
	mDepthImageMem(VK_NULL_HANDLE),
	mDepthImageView(VK_NULL_HANDLE),
	mQuad(mVulkanState)
{
	
}

VulkanManager::~VulkanManager()
{
}

void VulkanManager::init() 
{

	createVkInstance();

#ifdef AMVK_DEBUG
	enableDebug();
#endif

	createVkSurface(*mWindow.mGlfwWindow);
	createPhysicalDevice();
	createLogicalDevice();
	createSwapChain(mWindow);
	createImageViews();
	createRenderPass();
	

	createCommandPool();
	mQuad.init();



	createDepthResources();
	createFramebuffers();


	createCommandBuffers();
	createSemaphores();
	LOG("INIT SUCCESSFUL");

	//glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float) mSwapChainExtent.height, 0.1f, 10.0f);

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

	#ifdef AMVK_DEBUG
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	#endif

	return extensions;
}

void VulkanManager::createVkInstance() 
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

void VulkanManager::createVkSurface(GLFWwindow& glfwWindow)
{	
	VK_CHECK_RESULT(glfwCreateWindowSurface(mVulkanState.instance, &glfwWindow, nullptr, &mVulkanState.surface));
	
	if (glfwVulkanSupported() == GLFW_FALSE)
		throw std::runtime_error("Vulkan is not supported by GLFW. Cannot create surface");
}


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
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


void VulkanManager::enableDebug() 
{
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.pNext = nullptr;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT 
					 | VK_DEBUG_REPORT_WARNING_BIT_EXT
					 | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	createInfo.pfnCallback = &debugCallback;
	createInfo.pUserData = nullptr;

	VK_CALL_IPROC(mVulkanState.instance, vkCreateDebugReportCallbackEXT, mVulkanState.instance, &createInfo, nullptr, &mDebugReportCallback);
	LOG("DEBUG ENABLED");

}

void VulkanManager::createPhysicalDevice() 
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

		SwapChainDesc swapChainDesc = getSwapChainDesc(device, mVulkanState.surface);
		bool swapChainSupported = !swapChainDesc.mSurfaceFormats.empty() && !swapChainDesc.mPresentModes.empty();

		if (!swapChainSupported) 
			continue;

		if (dqi.graphicsIndexSet() && dqi.supportedIndexSet()) {
			mVulkanState.physicalDevice = device;
			mDeviceQueueIndices = dqi;
			mSwapChainDesc = swapChainDesc;
			LOG("DEVICE INITIALIZED");
			break;
		}
	}

	if (mVulkanState.physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No valid physical device found");
}

void VulkanManager::createLogicalDevice() 
{
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::unordered_set<int> uniqueIndices;
	
	for (size_t i = 0; i < 2; ++i) {
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
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, mVulkanState.surface, &surfaceSupported));

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
	createInfo.surface = mVulkanState.surface;
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

	VkSwapchainKHR oldSwapChain = mVulkanState.swapChain;

	createInfo.oldSwapchain = oldSwapChain;
	VkSwapchainKHR swapChain;
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mVulkanState.device, &createInfo, nullptr, &swapChain));
	mVulkanState.swapChain = swapChain;
	vkGetSwapchainImagesKHR(mVulkanState.device, mVulkanState.swapChain, &numImages, nullptr);
	mSwapChainImages.resize(numImages);
	vkGetSwapchainImagesKHR(mVulkanState.device, mVulkanState.swapChain, &numImages, mSwapChainImages.data());
	
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
	VkExtent2D extent;
	extent.width = std::max(
			surfaceCapabilities.minImageExtent.width, 
			std::min(window.getWidth(), surfaceCapabilities.maxImageExtent.width));
	extent.height = std::max(
			surfaceCapabilities.minImageExtent.height, 
			std::min(window.getHeight(), surfaceCapabilities.maxImageExtent.height));
	
	return extent;
}

void VulkanManager::createImageViews() 
{
	mSwapChainImageViews.resize(mSwapChainImages.size());
	for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
		createImageView(mSwapChainImages[i], mSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mSwapChainImageViews[i]);
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

	VkAttachmentDescription depthAtt = {};
	depthAtt.format = findDepthFormat();
	depthAtt.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAtt.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAtt.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAtt.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAtt.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference attRef = {};
	attRef.attachment = 0;
	attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentReference depthAttRef = {};
	depthAttRef.attachment = 1;
	depthAttRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription sub = {};
	sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	sub.colorAttachmentCount = 1;

	sub.pColorAttachments = &attRef;

	sub.pDepthStencilAttachment = &depthAttRef;
	
	VkSubpassDependency dependancy = {};
	dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependancy.dstSubpass = 0;
	dependancy.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependancy.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT 
							 | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {
		att, 
		depthAtt
	};
	
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = attachments.size();
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &sub;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependancy;

	VK_CHECK_RESULT(vkCreateRenderPass(mVulkanState.device, &createInfo, nullptr, &mVulkanState.renderPass));
	LOG("RENDER PASS CREATED");
}

void VulkanManager::createFramebuffers()
{
	mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = mVulkanState.renderPass;
	createInfo.width = mSwapChainExtent.width;
	createInfo.height = mSwapChainExtent.height;
	createInfo.layers = 1;

	for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i) {
		std::array<VkImageView, 2> attachments = { 
			mSwapChainImageViews[i],
			mDepthImageView
		};

		createInfo.attachmentCount = attachments.size();
		createInfo.pAttachments = attachments.data();

		VK_CHECK_RESULT(vkCreateFramebuffer(mVulkanState.device, &createInfo, nullptr, &mSwapChainFramebuffers[i]));
		LOG("FRAMEBUFFER CREATED");
	}
}

void VulkanManager::createCommandPool()
{
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = mDeviceQueueIndices.getGraphicsQueueIndex();
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK_RESULT(vkCreateCommandPool(mVulkanState.device, &createInfo, nullptr, &mVulkanState.commandPool));
	LOG("COMMAND BUFFER CREATED");
}

void VulkanManager::createCommandBuffers()
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

void VulkanManager::updateCommandBuffers(const Timer& timer, Camera& camera) 
{

	mQuad.updateUniformBuffers();

	VkClearValue clearValues[] = {
		{0.4f, 0.1f, 0.1f, 1.0f}, // VkClearColorValue color; 
		{1.0f, 0}				  // VkClearDepthStencilValue depthStencil 
	};

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = mVulkanState.renderPass;
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = mSwapChainExtent;
	renderPassBeginInfo.clearValueCount = ARRAY_SIZE(clearValues);
	renderPassBeginInfo.pClearValues = clearValues;
	
	for (size_t i = 0; i < mVkCommandBuffers.size(); ++i) {
		VK_CHECK_RESULT(vkBeginCommandBuffer(mVkCommandBuffers[i], &beginInfo));

		renderPassBeginInfo.framebuffer = mSwapChainFramebuffers[i];
		
		vkCmdBeginRenderPass(mVkCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(mVkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mQuad.mVkPipeline);
		mQuad.update(mVkCommandBuffers[i], timer, camera);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) mSwapChainExtent.width;
		viewport.height = (float) mSwapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = mSwapChainExtent;

		vkCmdSetViewport(mVkCommandBuffers[i], 0, 1, &viewport);
		vkCmdSetScissor(mVkCommandBuffers[i], 0, 1, &scissor);
		


		mQuad.draw(mVkCommandBuffers[i]);

		vkCmdEndRenderPass(mVkCommandBuffers[i]);

		VK_CHECK_RESULT(vkEndCommandBuffer(mVkCommandBuffers[i]));
	}
}

void VulkanManager::createSemaphores()
{
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_CHECK_RESULT(vkCreateSemaphore(mVulkanState.device, &createInfo, nullptr, &mImageAvailableSemaphore));
	VK_CHECK_RESULT(vkCreateSemaphore(mVulkanState.device, &createInfo, nullptr, &mRenderFinishedSemaphore));
	LOG("SEMAPHORES CREATED");
}

void VulkanManager::draw() 
{
	uint32_t imageIndex = 0;

	VkResult result = vkAcquireNextImageKHR(mVulkanState.device, 
										  mVulkanState.swapChain, 
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
		VkSwapchainKHR swapChains[] = { mVulkanState.swapChain };
		VkPipelineStageFlags stageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = stageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mVkCommandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK_RESULT(vkQueueSubmit(mVulkanState.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
		
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VK_CHECK_RESULT(vkQueuePresentKHR(mVulkanState.presentQueue, &presentInfo));

	} else {
		VK_THROW_RESULT_ERROR("Failed vkAcquireNextImageKHR", result);
	}
}

void VulkanManager::waitIdle() 
{
	vkDeviceWaitIdle(mVulkanState.device);
}

void VulkanManager::recreateSwapChain()
{
/*	vkQueueWaitIdle(mVulkanState.graphicsQueue);
	vkDeviceWaitIdle(mVulkanState.device);

	vkFreeCommandBuffers(mVulkanState.device, mVulkanState.commandPool, (uint32_t) mVkCommandBuffers.size(), mVkCommandBuffers.data());
	
	for (auto& framebuffer : mSwapChainFramebuffers)
		vkDestroyFramebuffer(mVulkanState.device, framebuffer, nullptr);

	vkDestroyImageView(mVulkanState.device, mDepthImageView, nullptr);
	vkDestroyImage(mVulkanState.device, mDepthImage, nullptr);
	vkFreeMemory(mVulkanState.device, mDepthImageMem, nullptr);
	
	vkDestroyPipeline(mVulkanState.device, mVkPipeline, nullptr);
	vkDestroyRenderPass(mVulkanState.device, mVulkanState.renderPass, nullptr);

	for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
		vkDestroyImageView(mVulkanState.device, mSwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mVulkanState.device, mVulkanState.swapChain, nullptr);

	createSwapChain(mWindow);
    createImageViews();
    createRenderPass();
    createPipeline();
	createDepthResources();
    createFramebuffers();
    createCommandBuffers();*/
}


uint32_t VulkanManager::getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags)
{
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(mVulkanState.physicalDevice, &memProps);

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

	VK_CHECK_RESULT(vkCreateBuffer(mVulkanState.device, &buffInfo, nullptr, &buffer));
	
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(mVulkanState.device, buffer, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, prop);
	
	VK_CHECK_RESULT(vkAllocateMemory(mVulkanState.device, &allocInfo, nullptr, &memory));
	vkBindBufferMemory(mVulkanState.device, buffer, memory, 0);
}

void VulkanManager::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
	VkCommandBuffer cmdBuf = beginSingleTimeCommands();

	VkBufferCopy bufferCopy = {};
	bufferCopy.size = size;

	vkCmdCopyBuffer(cmdBuf, src, dst, 1, &bufferCopy);

	endSingleTimeCommands(cmdBuf);
}

	
const VkDevice& VulkanManager::getVkDevice() const 
{
	return mVulkanState.device;
}
	
void VulkanManager::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer cmdBuf = beginSingleTimeCommands();
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format))

			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	} else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

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
	} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
			&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
							  | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
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

	VK_CHECK_RESULT(vkCreateImage(mVulkanState.device, &imageInfo, nullptr, &image));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(mVulkanState.device, image, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, properties);

	VK_CHECK_RESULT(vkAllocateMemory(mVulkanState.device, &allocInfo, nullptr, &imageMemory));

	vkBindImageMemory(mVulkanState.device, image, imageMemory, 0);
}



void VulkanManager::createImageView(
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		VkImageView& imageView)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;

	viewInfo.subresourceRange.aspectMask = aspectFlags;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK_RESULT(vkCreateImageView(mVulkanState.device, &viewInfo, nullptr, &imageView));
}




VkCommandBuffer VulkanManager::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mVulkanState.commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuf;
	vkAllocateCommandBuffers(mVulkanState.device, &allocInfo, &cmdBuf);
	
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

	vkQueueSubmit(mVulkanState.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mVulkanState.graphicsQueue);
	vkFreeCommandBuffers(mVulkanState.device, mVulkanState.commandPool, 1, &cmdBuf);
}

VkFormat VulkanManager::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(mVulkanState.physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR
		&& (props.linearTilingFeatures & features) == features) 
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL
		&& (props.optimalTilingFeatures & features) == features)
			return format;
	}
	throw new std::runtime_error("failed to find supported format");
}

VkFormat VulkanManager::findDepthFormat()
{
	return findSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool VulkanManager::hasStencilComponent(VkFormat format) 
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanManager::createDepthResources() 
{
	VkFormat depthFormat = findDepthFormat();
	createImage(
			mSwapChainExtent.width, 
			mSwapChainExtent.height, 
			depthFormat, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mDepthImage,
			mDepthImageMem);

	createImageView(mDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, mDepthImageView);
	transitionImageLayout(mDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}


