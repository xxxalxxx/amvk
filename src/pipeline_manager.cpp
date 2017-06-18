#include "pipeline_manager.h"

PipelineManager::PipelineManager(VulkanState& state):
	mState(state),
	mPipelines(state.pipelines)
{}


void PipelineManager::createPipelines() 
{
	createQuadPipeline();
}


void PipelineManager::createQuadPipeline()
{

}
