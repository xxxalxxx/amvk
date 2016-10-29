#ifndef AMVK_VULKAN_PIPELINE_CREATOR_H
#define AMVK_VULKAN_PIPELINE_CREATOR_H

#include "vulkan/vulkan.h"
#include "macro.h"

class VulkanPipelineCreator {
public:
	VulkanPipelineCreator();

	VkPipelineViewportStateCreateInfo viewportStateDynamic();
	VkPipelineViewportStateCreateInfo viewportStateDefault(VkExtent2D& extent);
	
	VkPipelineDynamicStateCreateInfo dynamicState(VkDynamicState* dynamicStates, uint32_t stateCount);

	VkPipelineShaderStageCreateInfo shaderStage(VkShaderModule& shaderModule, VkShaderStageFlagBits stage);
	
	VkPipelineColorBlendAttachmentState blendAttachmentStateDisabled();

	VkPipelineColorBlendStateCreateInfo blendStateDisabled(
			VkPipelineColorBlendAttachmentState* attachmentStates,
			uint32_t attachmentCount); 

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyNoRestart(VkPrimitiveTopology topology);

	VkPipelineMultisampleStateCreateInfo multisampleStateNoMultisampleNoSampleShading();

	VkPipelineDepthStencilStateCreateInfo depthStencilStateDepthLessNoStencil(); 


	VkPipelineRasterizationStateCreateInfo rasterizationStateCullBackCCW();

private:
};

#endif
