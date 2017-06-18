#include "shader_manager.h"


ShaderManager::ShaderManager(VulkanState& state):
	mState(state),
	mShaders(state.shaders)
{

}

void ShaderManager::createShaders()
{
	mShaders.quad.vertex = PipelineCreator::shaderStage(mState.device, "quad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	mShaders.quad.fragment = PipelineCreator::shaderStage(mState.device, "quad.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
}
