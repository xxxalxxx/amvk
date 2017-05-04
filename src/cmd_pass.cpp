#include "cmd_pass.h"

CmdPass::CmdPass(const VkDevice& vkDevice, const VkCommandPool& vkCommandPool, const VkQueue& vkQueue): 
	cmdBuffer(VK_NULL_HANDLE), mVkDevice(vkDevice), mVkCommandPool(vkCommandPool), mVkQueue(vkQueue)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vkDevice, &allocInfo, &cmdBuffer);
	
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
}

CmdPass::~CmdPass()
{
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType =  VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	vkQueueSubmit(mVkQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mVkQueue);
	vkFreeCommandBuffers(mVkDevice, mVkCommandPool, 1, &cmdBuffer);
}

VkCommandBuffer CmdPass::commandBuffer() 
{
	return cmdBuffer;
}
