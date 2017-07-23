#ifndef AMVK_PIPELINE_MANAGER_H
#define AMVK_PIPELINE_MANAGER_H

#include <cstddef>
#include <vector>
#include "macro.h"
#include "vulkan_state.h"
#include "pipeline_creator.h"
#include "pipeline_cache.h"
#include "tquad.h"
#include "model.h"
#include "skinned.h"
#include "point_light.h"
#include "g_buffer.h"

namespace PipelineManager
{

inline void createTQuadPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)
{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.tquad.vertex,
            state.shaders.tquad.fragment
    };

    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(TQuad::Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //location, binding, format, offset
    VkVertexInputAttributeDescription attrDesc[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(TQuad::Vertex, pos) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(TQuad::Vertex, color) },
            { 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(TQuad::Vertex, texCoord) }
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(&bindingDesc, 1, attrDesc, ARRAY_SIZE(attrDesc));

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
		//PipelineCreator::rasterizationStateCullNone();
		PipelineCreator::rasterizationStateCullBackCCW();
		//PipelineCreator::rasterizationStateCullBackCW();
    VkPipelineDepthStencilStateCreateInfo depthStencil = PipelineCreator::depthStencilStateDepthLessOrEqualNoStencil();
    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState = PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkPushConstantRange pushConstantRange = PipelineCreator::pushConstantRange(
            state,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(TQuad::PushConstants));

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(&state.descriptorSetLayouts.tquad, 1, &pushConstantRange, 1);
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("tquad", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);
}


inline void createFullscreenQuadPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)
{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.fullscreenQuad.vertex,
            state.shaders.fullscreenQuad.fragment
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(nullptr, 0, nullptr, 0);

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
		//PipelineCreator::rasterizationStateCullNone();
		//PipelineCreator::rasterizationStateCullBackCCW();
		PipelineCreator::rasterizationStateCullBackCW();
    VkPipelineDepthStencilStateCreateInfo depthStencil = PipelineCreator::depthStencilStateDepthLessOrEqualNoStencil();
    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState = PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(&state.descriptorSetLayouts.sampler, 1, nullptr, 0);
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("fullscreen_quad", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);
}

inline void createDeferredPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)
{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.deferred.vertex,
            state.shaders.deferred.fragment
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(nullptr, 0, nullptr, 0);

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
		//PipelineCreator::rasterizationStateCullNone();
		//PipelineCreator::rasterizationStateCullBackCCW();
		PipelineCreator::rasterizationStateCullBackCW();
    VkPipelineDepthStencilStateCreateInfo depthStencil = PipelineCreator::depthStencilStateDepthLessOrEqualNoStencil();
    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState = PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(&state.descriptorSetLayouts.deferred, 1, nullptr, 0);
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("deferred", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);
}

inline void createPointLightPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)

{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.pointLight.vertex,
            state.shaders.pointLight.fragment
    };

    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Sphere::Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //location, binding, format, offset
    VkVertexInputAttributeDescription attrDesc[] = {
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Sphere::Vertex, position) }
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(&bindingDesc, 1, attrDesc, ARRAY_SIZE(attrDesc));

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
		PipelineCreator::rasterizationStateCullNone();

	//	PipelineCreator::rasterizationStateWireframeCullNoneCW();
	//	PipelineCreator::rasterizationStateCullBackCCW();
		//PipelineCreator::rasterizationStateCullBackCW();
    VkPipelineDepthStencilStateCreateInfo depthStencil = 
		//PipelineCreator::depthStencilStateGBufferStencilPass();
		PipelineCreator::depthStencilStateDepthLessOrEqualNoStencil();
    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState = 
		PipelineCreator::blendAttachmentSrcAlpha();
		//PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = 
		PipelineCreator::blendStateEnabled(&blendAttachmentState, 1);
		//PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkDescriptorSetLayout layouts[] = {
		state.descriptorSetLayouts.uniformVertex,
		state.descriptorSetLayouts.dynamicUniformVertex,
		state.descriptorSetLayouts.dynamicUniformFragment,
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(layouts, ARRAY_SIZE(layouts), NULL, 0); 
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("point_light", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);
}


inline void createModelPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)
{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.model.vertex,
            state.shaders.model.fragment
    };

    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Model::Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //location, binding, format, offset
    VkVertexInputAttributeDescription attrDesc[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, pos) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, normal) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, tangent) },
            { 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model::Vertex, bitangent) },
            { 4, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Model::Vertex, texCoord) }
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(&bindingDesc, 1, attrDesc, ARRAY_SIZE(attrDesc));

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
		PipelineCreator::rasterizationState(
				VK_CULL_MODE_BACK_BIT, 
				VK_FRONT_FACE_COUNTER_CLOCKWISE, 
				VK_POLYGON_MODE_FILL, 
				VK_FALSE);
    //VkPipelineDepthStencilStateCreateInfo depthStencil = 
	//	PipelineCreator::depthStencilStateGBufferStencilPass();
	//	PipelineCreator::depthStencilStateGBufferLightPass();
		//PipelineCreator::depthStencilStateDepthLessNoStencil();


	VkStencilOpState front = {};
	front.compareOp = VK_COMPARE_OP_EQUAL;
	front.passOp = VK_STENCIL_OP_KEEP;
	front.failOp = VK_STENCIL_OP_KEEP;
	front.depthFailOp = VK_STENCIL_OP_KEEP;
	front.writeMask = 0x0;
	front.compareMask = 0xffffffff;
	front.reference = 1;

	VkStencilOpState back = {};
	back.compareOp = VK_COMPARE_OP_NEVER;
	back.passOp = VK_STENCIL_OP_KEEP;
	back.failOp = VK_STENCIL_OP_KEEP;
	back.depthFailOp = VK_STENCIL_OP_KEEP;
	back.writeMask = 0x0;
	back.compareMask = 0xffffffff;
	back.reference = 1;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = front;
	depthStencil.back = back;



    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState =
		//PipelineCreator::blendAttachmentSrcAlpha();
			PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = 
		//PipelineCreator::blendStateEnabled(&blendAttachmentState, 1);
		PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkDescriptorSetLayout layouts[] = {
            state.descriptorSetLayouts.uniformVertex,
            state.descriptorSetLayouts.sampler
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(layouts, ARRAY_SIZE(layouts), NULL, 0);
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("model", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);

    LOG("MODEL PIPELINE CREATED");

}


inline void createSkinnedPipeline(VulkanState& state, PipelineInfo& info, VkRenderPass renderPass, uint32_t subpass)
{
    VkPipelineShaderStageCreateInfo stages[] = {
            state.shaders.skinned.vertex,
            state.shaders.skinned.fragment
    };

    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Skinned::Vertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //location, binding, format, offset
    VkVertexInputAttributeDescription attrDesc[] = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Skinned::Vertex, pos) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Skinned::Vertex, normal) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Skinned::Vertex, tangent) },
            { 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Skinned::Vertex, bitangent) },
            { 4, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Skinned::Vertex, texCoord) },
            { 5, 0, VK_FORMAT_R32G32B32A32_UINT, offsetof(Skinned::Vertex, boneIndices) },
            { 6, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Skinned::Vertex, weights) },
            { 7, 0, VK_FORMAT_R32G32B32A32_UINT, offsetof(Skinned::Vertex, samplerIndices) },
    };

    auto vertexInputInfo = PipelineCreator::vertexInputState(&bindingDesc, 1, attrDesc, ARRAY_SIZE(attrDesc));

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
    VkPipelineRasterizationStateCreateInfo rasterizationState = 
			PipelineCreator::rasterizationState(
				VK_CULL_MODE_BACK_BIT, 
				VK_FRONT_FACE_COUNTER_CLOCKWISE, 
				VK_POLYGON_MODE_FILL);
		//PipelineCreator::rasterizationStateCullBackCCW();
   // VkPipelineDepthStencilStateCreateInfo depthStencil = 
	//	PipelineCreator::depthStencilStateGBufferLightPass();
		//PipelineCreator::depthStencilStateDepthLessNoStencil();





	VkStencilOpState front = {};
	front.compareOp = VK_COMPARE_OP_ALWAYS;
	front.passOp = VK_STENCIL_OP_INCREMENT_AND_WRAP;
	front.failOp = VK_STENCIL_OP_KEEP;
	front.depthFailOp = VK_STENCIL_OP_KEEP;
	front.writeMask = 0xffffffff;
	front.compareMask = 0xffffffff;
	front.reference = 0;
	
	VkStencilOpState back = {};
	back.compareOp = VK_COMPARE_OP_ALWAYS;
	back.passOp = VK_STENCIL_OP_KEEP;
	back.failOp = VK_STENCIL_OP_KEEP;
	back.depthFailOp = VK_STENCIL_OP_KEEP;
	back.writeMask = 0xff;
	back.compareMask = 0xffffffff;
	back.reference = 0;



	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = front;
	depthStencil.back = back;



    VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
    VkPipelineColorBlendAttachmentState blendAttachmentState = 
		//PipelineCreator::blendAttachmentSrcAlpha();
		PipelineCreator::blendAttachmentStateDisabled();

    VkPipelineColorBlendStateCreateInfo blendState = 
		//	PipelineCreator::blendStateEnabled(&blendAttachmentState, 1);
		PipelineCreator::blendStateDisabled(&blendAttachmentState, 1);

    VkDescriptorSetLayout layouts[] = {
            state.descriptorSetLayouts.uniformVertex,
            state.descriptorSetLayouts.samplerList
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(layouts, ARRAY_SIZE(layouts), NULL, 0);
    VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &info.layout));

    PipelineCacheInfo cacheInfo("skinned", info.cache);
    cacheInfo.getCache(state.device);

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE(stages);
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &blendState;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = info.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &info.pipeline));

    cacheInfo.saveCache(state.device);

    LOG("SKINNED MODEL PIPELINE CREATED");

}


inline void createPipelines(VulkanState& state, GBuffer& gBuffer)
{
    createTQuadPipeline(state, state.pipelines.tquad, state.renderPass, 0);
	createPointLightPipeline(state, state.pipelines.pointLight, state.renderPass, 0);
	createFullscreenQuadPipeline(state, state.pipelines.fullscreenQuad, state.renderPass, 0);
    createModelPipeline(state, state.pipelines.model, state.renderPass, 0);
    createSkinnedPipeline(state, state.pipelines.skinned, state.renderPass, 0);
    createDeferredPipeline(state, state.pipelines.deferred, state.renderPass, 0);

}

};


#endif
