#ifndef AMVK_SHADER_MANAGER_H
#define AMVK_SHADER_MANAGER_H

#include "macro.h"
#include "vulkan_state.h"
#include "pipeline_creator.h"


namespace ShaderManager 
{

	
inline void createShaders(VulkanState& state)
{
	Shaders& shaders = state.shaders;
	
	shaders.tquad.vertex = PipelineCreator::shaderStage(state.device, "tquad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaders.tquad.fragment = PipelineCreator::shaderStage(state.device, "tquad.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	shaders.model.vertex = PipelineCreator::shaderStage(state.device, "model.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaders.model.fragment = PipelineCreator::shaderStage(state.device, "model.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	shaders.skinned.vertex = PipelineCreator::shaderStage(state.device, "skinned.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaders.skinned.fragment = PipelineCreator::shaderStage(state.device, "skinned.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	shaders.fullscreenQuad.vertex = PipelineCreator::shaderStage(state.device, "fullscreen_quad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaders.fullscreenQuad.fragment = PipelineCreator::shaderStage(state.device, "fullscreen_quad.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	
	shaders.pointLight.vertex = PipelineCreator::shaderStage(state.device, "point_light.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaders.pointLight.fragment = PipelineCreator::shaderStage(state.device, "point_light.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
}


};

#endif
