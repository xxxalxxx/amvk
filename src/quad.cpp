#include "quad.h"

Quad::Quad(VulkanManager& vulkanManager):
	mVulkanManager(vulkanManager),
	mNumIndices(6)
{

}

VkVertexInputBindingDescription Quad::getBindingDesc()
{
	VkVertexInputBindingDescription bindDesc = {};
	bindDesc.binding = 0;
	bindDesc.stride = sizeof(Vertex);
	bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindDesc;
}

std::array<VkVertexInputAttributeDescription, 2> Quad::getAttrDesc() 
{
	std::array<VkVertexInputAttributeDescription, 2> vertexInputAttrDesc = {{
		{0, 0, VK_FORMAT_R32G32_SFLOAT, (uint32_t) offsetof(Vertex, pos) },
		{0, 1, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t) offsetof(Vertex, color) }
	}};
	return vertexInputAttrDesc;
}

void Quad::initBuffers() 
{
	createVertexBuffer();
	createIndexBuffer();
}


void Quad::createVertexBuffer() 
{

	const Vertex vertices[] = {
	    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	VkDeviceSize bufSize = sizeof(vertices);
	VkBuffer stagingBuf;
	VkDeviceMemory stagingBufDeviceMem;
	mVulkanManager.createBuffer(stagingBuf, 
								bufSize, 
								stagingBufDeviceMem, 
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);	
	void* data;
	vkMapMemory(mVulkanManager.getVkDevice(), stagingBufDeviceMem, 0, bufSize, 0 , &data);
	memcpy(data, &vertices[0], (size_t) bufSize);
	vkUnmapMemory(mVulkanManager.getVkDevice(), stagingBufDeviceMem);
	mVulkanManager.createBuffer(vertexBuffer, 
								bufSize, 
								vertexBufferMem, 
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
								VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	mVulkanManager.copyBuffer(stagingBuf, vertexBuffer, bufSize);
}

void Quad::createIndexBuffer()
{
	const int indices[] = {
		0, 1, 2, 
		2, 3, 0
	};

	VkDeviceSize bufSize = sizeof(indices);
	VkBuffer stagingBuf;
	VkDeviceMemory stagingBufDeviceMem;
	mVulkanManager.createBuffer(stagingBuf, 
								bufSize, 
								stagingBufDeviceMem, 
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);	
	void* data;
	vkMapMemory(mVulkanManager.getVkDevice(), stagingBufDeviceMem, 0, bufSize, 0 , &data);
	memcpy(data, &indices[0], (size_t) bufSize);
	vkUnmapMemory(mVulkanManager.getVkDevice(), stagingBufDeviceMem);
	mVulkanManager.createBuffer(indexBuffer, 
								bufSize, 
								indexBufferMem, 
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
								VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	mVulkanManager.copyBuffer(stagingBuf, indexBuffer, bufSize);
}




