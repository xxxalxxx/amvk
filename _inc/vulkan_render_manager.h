#ifndef AMVK_VULKAN_RENDER_MANAGER_H
#define AMVK_VULKAN_RENDER_MANAGER_H

#include <vulkan/vulkan.h>

#include "vulkan_state.h"
#include "vulkan_render_pass_creator.h"

class VulkanRenderManager {
public:
	VulkanRenderManager(VulkanState& vulkanState);
	~VulkanRenderManager();

private:
	VulkanState& mVulkanState;
};


#endif
