#ifndef AMVK_PIPELINE_MANAGER_H
#define AMVK_PIPELINE_MANAGER_H

#include <cstddef>
#include "macro.h"
#include "vulkan_state.h"
#include "pipeline_creator.h"

class PipelineManager {
public:
	PipelineManager(VulkanState& state);
	void createPipelines();
private:
	void createQuadPipeline();
	VulkanState& mState;
	Pipelines& mPipelines;
};


#endif
