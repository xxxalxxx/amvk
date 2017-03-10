#ifndef AMVK_VULKAN_PIPELINE_CREATOR_H
#define AMVK_VULKAN_PIPELINE_CREATOR_H

#include <vulkan/vulkan.h>
#include "macro.h"
#include "file_manager.h"
#include "vulkan_utils.h"
#include "vulkan_state.h"

class PipelineCreator {
public:

	static VkPipelineViewportStateCreateInfo viewportStateDynamic();
	static VkPipelineViewportStateCreateInfo viewportStateDefault(VkExtent2D& extent);
	
	static VkPipelineDynamicStateCreateInfo dynamicState(VkDynamicState* dynamicStates, uint32_t stateCount);

	static VkPipelineShaderStageCreateInfo shaderStage(VkShaderModule& shaderModule, VkShaderStageFlagBits stage);
	static VkPipelineShaderStageCreateInfo shaderStage(const VkDevice& device, const char* path, VkShaderStageFlagBits stage);
	static VkPipelineColorBlendAttachmentState blendAttachmentStateDisabled();

	static VkPushConstantRange pushConstantRange(
			const VulkanState& state, 
			VkShaderStageFlags stageFlags, 
			uint32_t offset, 
			uint32_t size); 

	static VkPipelineColorBlendStateCreateInfo blendStateDisabled(
			VkPipelineColorBlendAttachmentState* attachmentStates,
			uint32_t attachmentCount); 

	static VkPipelineInputAssemblyStateCreateInfo inputAssemblyNoRestart(VkPrimitiveTopology topology);

	static VkPipelineMultisampleStateCreateInfo multisampleStateNoMultisampleNoSampleShading();

	static VkPipelineDepthStencilStateCreateInfo depthStencilStateDepthLessNoStencil(); 


	static VkPipelineRasterizationStateCreateInfo rasterizationStateCullBackCCW();

	static VkPipelineLayoutCreateInfo layout(VkDescriptorSetLayout* setLayouts, uint32_t setLayoutCount,
			VkPushConstantRange* pushConstantRanges,uint32_t pushConstantRangeCount);
};

#endif
