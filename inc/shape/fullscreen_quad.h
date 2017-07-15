#ifndef AMVK_FULLSCREEN_QUAD_H
#define AMVK_FULLSCREEN_QUAD_H

/*
 * Fullscreen quad inside triangle is courtesy of Sascha Willems.
 * His tutorial can be found at https://www.saschawillems.de/?page_id=2122
 */

#include "macro.h"

#ifdef __ANDROID__
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif

#include "vulkan_state.h"
#include "texture_manager.h"

class FullscreenQuad {
public:
	FullscreenQuad(VulkanState& state);
	void init();
	void draw(VkCommandBuffer& cmdBuffer); 
private:
	void createDescriptorSets();
	VulkanState& mState;
	VkDescriptorSet mDescriptorSet;
	ImageInfo* mImageInfo;
};

#endif
