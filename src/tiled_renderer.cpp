#include "tiled_renderer.h"

TiledRenderer::TiledRenderer(State& state, GBuffer& gBuffer): 
	mState(state),
	mGBuffer(gBuffer)
{

}

TiledRenderer::~TiledRenderer() 
{

}

void TiledRenderer::createCmdPool() 
{
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = mState.computeQueueIndex;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK_RESULT(vkCreateCommandPool(mState.device, &createInfo, nullptr, &mState.commandPool));
}

void TiledRenderer::createCmdBuffer()
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = cmdPool;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
	cmdBufferAllocInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(mState.device, &cmdBufferAllocInfo, &cmdBuffer));
}

void TiledRenderer::createDescriptorPool() 
{
	// VkDescriptorType    type;
	// uint32_t            descriptorCount;
	uint32_t maxSets = GBuffer::ATTACHMENT_COUNT + 2 + pointLights.size();
	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxSets }
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = ARRAY_SIZE(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = maxSets;

	VK_CHECK_RESULT(vkCreateDescriptorPool(mState.device, &poolInfo, nullptr, &descriptorPool));
}

void TiledRenderer::createDescriptorSet()
{

}

void TiledRenderer::createStoreImages() 
{

}

void TiledRenderer::createUniformBuffers() 
{

}


void TiledRenderer::init() 
{
	LOG_TITLE("Tiled renderer");

	createCmdPool();
	createCmdBuffer();
	createDescriptorPool();
	createDescriptorSet();
}

void TiledRenderer::update(VkCommandBuffer& cmdBuffer, const Timer& timer, Camera& camera)
{

}

void TiledRenderer::draw() 
{
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mState.pipelines.tiling.pipeline);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mState.pipelines.tiling.layout, 0, 1, &descriptorSet, 0, 0);
	vkCmdDispatch(cmdBuffer, WORK_GROUP_SIZE, WORK_GROUP_SIZE, 1);
}
