#ifndef AMVK_TILED_RENDERER_H
#define AMVK_TILED_RENDERER_H

#include <vector>

#include "macro.h"
#include "vulkan.h"
#include "vulkan_utils.h"
#include "state.h"
#include "camera.h"
#include "timer.h"
#include "g_buffer.h"

class TiledRenderer {
public:
	static const constexpr uint32_t MAX_LIGHTS = 1024;
	static const constexpr uint32_t WORK_GROUP_SIZE = 16;

	struct UBO {
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 eyePos;
		glm::vec2 textureDimens;
	};

	struct PointLight {
		glm::vec3 position;
		float radius;
		glm::vec3 color;
		float intensity;
	};

	TiledRenderer(State& state, GBuffer& gBuffer);
	virtual ~TiledRenderer();
	void init();
	void update(VkCommandBuffer& cmdBuffer, const Timer& timer, Camera& camera);
	void draw();

	VkCommandBuffer cmdBuffer;
	VkCommandPool cmdPool;

	UBO ubo;
	std::vector<PointLight> pointLights;
private:
	void createStoreImages();
	void createUniformBuffers();
	void createCmdPool();
	void createCmdBuffer();
	void createDescriptorPool();
	void createDescriptorSet();

	State& mState;
	GBuffer& mGBuffer;

	VkDescriptorSet descriptorSet;
	VkDescriptorPool descriptorPool;
};

#endif
