#ifndef AMVK_VULKAN_PIPELINE_CREATOR_H
#define AMVK_VULKAN_PIPELINE_CREATOR_H

#include <vulkan/vulkan.h>
#include "macro.h"
#include "file_manager.h"
#include "vulkan_utils.h"

class VulkanPipelineCreator {
public:
	VulkanPipelineCreator();

	VkPipelineViewportStateCreateInfo viewportStateDynamic();
	VkPipelineViewportStateCreateInfo viewportStateDefault(VkExtent2D& extent);
	
	VkPipelineDynamicStateCreateInfo dynamicState(VkDynamicState* dynamicStates, uint32_t stateCount);

	VkPipelineShaderStageCreateInfo shaderStage(VkShaderModule& shaderModule, VkShaderStageFlagBits stage);
	VkPipelineShaderStageCreateInfo shaderStage(const VkDevice& device, const char* path, VkShaderStageFlagBits stage);
	VkPipelineColorBlendAttachmentState blendAttachmentStateDisabled();

	VkPipelineColorBlendStateCreateInfo blendStateDisabled(
			VkPipelineColorBlendAttachmentState* attachmentStates,
			uint32_t attachmentCount); 

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyNoRestart(VkPrimitiveTopology topology);

	VkPipelineMultisampleStateCreateInfo multisampleStateNoMultisampleNoSampleShading();

	VkPipelineDepthStencilStateCreateInfo depthStencilStateDepthLessNoStencil(); 


	VkPipelineRasterizationStateCreateInfo rasterizationStateCullBackCCW();

	VkPipelineLayoutCreateInfo layout(VkDescriptorSetLayout* setLayouts, uint32_t setLayoutCount,
			VkPushConstantRange* pushConstantRanges,uint32_t pushConstantRangeCount);

private:
};

#endif
