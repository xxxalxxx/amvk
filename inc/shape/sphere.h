#ifndef AMVK_SPHERE_H
#define AMVK_SPHERE_H

#ifdef __ANDROID__
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif

#include <stdint.h>
#include <cmath>
#include <vector>
#include <glm/vec3.hpp>

#include "macro.h"
#include "vulkan_state.h"
#include "buffer_helper.h"

class Sphere {
public:
	struct Vertex {
		glm::vec3 position;
	};

	Sphere(VulkanState& state);
	void init(uint32_t numStacks, uint32_t numSlices, float radius = 1.0f);
	uint32_t numIndices;
	float getRadius() const;

	VkDeviceSize vertexBufferOffset, indexBufferOffset;
	BufferInfo mCommonBufferInfo;
private:
	void createBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	VulkanState& mState;

	float mRadius;
};

#endif
