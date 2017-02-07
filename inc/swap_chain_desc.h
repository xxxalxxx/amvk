#ifndef AMVK_SWAP_CHAIN_DESC_H
#define AMVK_SWAP_CHAIN_DESC_H

#include <vulkan/vulkan.h>
#include <vector>

class SwapChainDesc {
public:
	SwapChainDesc();
	bool supported() const;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif
