#ifndef AMVK_G_BUFFER_H
#define AMVK_G_BUFFER_H



#include "macro.h"
#include "vulkan.h"
#include "buffer_helper.h"
#include "image_helper.h"
#include "state.h"
#include "fullscreen_quad.h"
#include "camera.h"
#include "timer.h"
#include "utils.h"

#include <cfloat>
#include <array>
#include <unordered_set>
#include <glm/gtx/string_cast.hpp> 

struct FramebufferAttachment {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
	VkFormat format;
};

class GBuffer {
public:
	static const constexpr uint32_t INDEX_POSITION = 0;
	static const constexpr uint32_t INDEX_NORMAL   = 1;
	static const constexpr uint32_t INDEX_ALBEDO   = 2;
	static const constexpr uint32_t INDEX_DEPTH    = 3;

	static const constexpr uint32_t INDEX_TILING_OUT_IMAGE    = 0;
	static const constexpr uint32_t INDEX_TILING_NORMAL_DEPTH = 1;
	static const constexpr uint32_t INDEX_TILING_POSITION     = 2;
	static const constexpr uint32_t INDEX_TILING_ALBEDO       = 3;
	static const constexpr uint32_t INDEX_TILING_UBO          = 4;
	static const constexpr uint32_t INDEX_TILING_POINT_LIGHTS = 5;

	static const constexpr uint32_t DEPTH_ATTACHMENT_COUNT = 1;
	static const constexpr uint32_t ATTACHMENT_COUNT = 4;
	static const constexpr uint32_t COLOR_ATTACHMENT_COUNT = ATTACHMENT_COUNT - DEPTH_ATTACHMENT_COUNT;
	static const constexpr uint32_t TILING_IMAGE_COUNT = 4;
	static const constexpr uint32_t UNIFORM_BUFFER_COUNT = 2;
	static const constexpr uint32_t STORAGE_BUFFER_COUNT = 1;

	static const constexpr uint32_t MAX_LIGHTS = 1024;
	static const constexpr uint32_t WORK_GROUP_SIZE = 16;

	struct TilingUBO {
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec2 textureDimens;
		uint32_t lightCount;
	};

	struct PointLight {
		glm::vec3 position;
		float radius;
		glm::vec3 color;
		float pad;
	};

	inline FramebufferAttachment& position() { return attachments[INDEX_POSITION]; }
	inline FramebufferAttachment& normal() { return attachments[INDEX_NORMAL]; }
	inline FramebufferAttachment& albedo() { return attachments[INDEX_ALBEDO]; }
	inline FramebufferAttachment& depth() { return attachments[INDEX_DEPTH]; }

	GBuffer(State& state);
	virtual ~GBuffer();

	void init(const VkPhysicalDevice& physicalDevice, const VkDevice& device, uint32_t width, uint32_t height);
	void createCmdBuffer(const VkDevice& device, const VkCommandPool& cmdPool);
	void drawDeferredQuad(VkCommandBuffer& cmdBuffer);
	void dispatch();
	void updateTextureDimens(uint32_t width, uint32_t height);
	void updateTiling(VkCommandBuffer& cmdBuffer, const Timer& timer, Camera& camera);
	void update(VkCommandBuffer& cmdBuffer, const Timer& timer, Camera& camera);

	std::array<FramebufferAttachment, ATTACHMENT_COUNT> attachments;
	std::array<VkClearValue, ATTACHMENT_COUNT> clearValues;
	int32_t width, height;
	VkFramebuffer frameBuffer;
	VkRenderPass renderPass;
	VkSampler sampler;
	VkCommandBuffer cmdBuffer;

	VkCommandBuffer tilingCmdBuffer;
	VkCommandPool tilingCmdPool;

	TilingUBO ubo;
	std::vector<PointLight> pointLights;


	struct TilingImage {
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
	} tilingImage;

private:
	void createFramebuffers(const VkPhysicalDevice& physicalDevice, const VkDevice& device);
	
	void createBuffers();
	void createAttachment(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& device,
		FramebufferAttachment& attachment,
		VkFormat format,  
		VkImageUsageFlags usage);
	void createTilingResultImage(); 
	void createTilingCmdPool();
	void createSampler(const VkDevice& device);
	void createDescriptorPool();
	void createDescriptors();
	void createColorAttachmentDesc(VkAttachmentDescription& desc, VkFormat format);
	void createDepthAttachmentDesc(VkAttachmentDescription& desc, VkFormat format);
	void createLights();
    VkImageTiling getTiling(VkFormat format, VkImageUsageFlags usage);

	void initColorImageTransition(CmdPass& cmd, FramebufferAttachment& attachment);

	State* mState;
	VkDescriptorPool mDescriptorPool;
	VkDescriptorSet mDescriptorSet;
	VkDescriptorSet mTilingDescriptorSet;
	BufferInfo mUniformBufferInfo;
	BufferInfo mTilingUniformBufferInfo;
	BufferInfo mTilingStagingUniformBufferInfo;
	BufferInfo mPointLightsBufferInfo;
public:
	FullscreenQuad deferredQuad;
};


#endif
