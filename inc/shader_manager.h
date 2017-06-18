#ifndef AMVK_SHADER_MANAGER_H
#define AMVK_SHADER_MANAGER_H

#include "macro.h"
#include "vulkan_state.h"
#include "pipeline_creator.h"

class ShaderManager {
public:
	ShaderManager(VulkanState& state);
	void createShaders();
private:
	VulkanState& mState;
	Shaders& mShaders;
};

#endif
