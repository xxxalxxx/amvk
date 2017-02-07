#ifndef AMVK_VULKAN_RENDER_PASS_CREATOR_H
#define AMVK_VULKAN_RENDER_PASS_CREATOR_H

#include "vulkan/vulkan.h"

class VulkanRenderPassCreator {
public:
	VulkanRenderPassCreator();
	~VulkanRenderPassCreator();
		
	VkAttachmentDescription attachmentDescColor(VkFormat format) const;
	VkAttachmentDescription attachmentDescDepthNoStencil(VkFormat format) const; 
};

#endif
