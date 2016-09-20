#ifndef AMVK_QUAD_H
#define AMVK_QUAD_H

#include <cstring>
#include <cstddef>
#include <string.h>
#include <array>
#include "macro.h"
#include "engine.h"
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

class Quad {
public:
	Quad(VkDevice& mVkDevice);

	void initBuffers();
	void draw();	
	VkVertexInputBindingDescription getBindingDesc();
	std::array<VkVertexInputAttributeDescription, 2> getAttrDesc();
	void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize size);

	void createBuffer(VkBuffer& buffer, 
					  VkDeviceSize size, 
					  VkDeviceMemory& memory, 
					  VkMemoryPropertyFlags prop, 
					  VkBufferUsageFlags usage);

	uint32_t getMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags& flags);
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
	};

	VkBuffer mVertexBuffer, mIndexBuffer;
	uint32_t mNumIndices;
private:
	void createVertexBuffer();
	void createIndexBuffer();
	VkDevice mVkDevice;
	VkDeviceMemory vertexBufferMem, indexBufferMem;
};

#endif

