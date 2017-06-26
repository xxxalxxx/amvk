#ifndef AMVK_DESCRIPTOR_MANAGER_H
#define AMVK_DESCRIPTOR_MANAGER_H

#include "macro.h"
#include "vulkan_state.h"
#include "pipeline_creator.h"

class DescriptorManager {
public:
	DescriptorManager(VulkanState& state);
	void createDescriptorSetLayouts();
	void createDescriptorPool();

private:
	void createQuadDescriptorSetLayout();
	void createModelDescriptorSetLayout();
	void createSamplerDescriptorSetLayout();
	void createUniformDescriptorSetLayout();
	VulkanState& mState;
	DescriptorSetLayouts& mLayouts;
};

#endif
