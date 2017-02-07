#include "vulkan_buffer_creator.h"

VulkanBufferDesc::VulkanBufferDesc(const VkDevice& device):
	buffer(VK_NULL_HANDLE),
	memory(VK_NULL_HANDLE),
	mVkDevice(device)
{

}
VulkanBufferDesc::~VulkanBufferDesc() 
{
	vkDestroyBuffer(mVkDevice, buffer, nullptr);
	vkFreeMemory(mVkDevice, memory, nullptr);
}



VulkanBufferCreator::~VulkanBufferCreator()
{

}

VulkanBufferCreator::VulkanBufferCreator(const VulkanState& vulkanState):
   mVulkanState(vulkanState)
{

}

uint32_t VulkanBufferCreator::getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags) const
{
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(mVulkanState.physicalDevice, &memProps);

	for (size_t i = 0; i < memProps.memoryTypeCount; ++i)
		if ((typeFilter & (1 << i)) 
		&& (memProps.memoryTypes[i].propertyFlags & flags))
			return i;
	throw std::runtime_error("Failed to find memory type");
}


void VulkanBufferCreator::createBuffer(
		VkBuffer& buffer, 
		VkDeviceSize size, 
		VkDeviceMemory& memory, 
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags prop) const
{
	VkBufferCreateInfo buffInfo = {};
	buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffInfo.size = size;
	buffInfo.usage = usage;
	buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_RESULT(vkCreateBuffer(mVulkanState.device, &buffInfo, nullptr, &buffer));
	
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(mVulkanState.device, buffer, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = getMemoryType(memReqs.memoryTypeBits, prop);
	
	VK_CHECK_RESULT(vkAllocateMemory(mVulkanState.device, &allocInfo, nullptr, &memory));
	vkBindBufferMemory(mVulkanState.device, buffer, memory, 0);
}

void VulkanBufferCreator::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) const
{
	CmdPass cmdPass(mVulkanState.device, mVulkanState.commandPool, mVulkanState.graphicsQueue);

	VkBufferCopy bufferCopy = {};
	bufferCopy.size = size;

	vkCmdCopyBuffer(cmdPass.commandBuffer(), src, dst, 1, &bufferCopy);
}
