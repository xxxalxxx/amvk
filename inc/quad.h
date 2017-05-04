#ifndef AMVK_QUAD_H
#define AMVK_QUAD_H
#include "macro.h"

#include <vulkan/vulkan.h>
#include <cstring>
#include <cstddef>
#include <string.h>
#include <array>
#include <vector>

#include "buffer_helper.h"
#include "vulkan_image_creator.h"
#include "vulkan_render_pass_creator.h"
#include "texture_manager.h"
#include "pipeline_creator.h"
#include "timer.h"
#include "camera.h"

class Quad {
public:
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	struct UBO {
	    glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct PushConstants {
	    glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	Quad(const VulkanState& vulkanState);
	~Quad();
	void draw(VkCommandBuffer& commandBuffer); 
	void init();
	void update(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera);
	void updateUniformBuffers(const Timer& timer, Camera& camera); 
	void updatePushConstants(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera);

	VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo(
		VkVertexInputBindingDescription& bindingDesc, 
		std::array<VkVertexInputAttributeDescription, 3>& attrDesc) const;

	uint32_t numIndices;
	
	VkDeviceSize mVertexBufferOffset, 
				 mIndexBufferOffset, 
				 mUniformBufferOffset;

	VkPipeline mVkPipeline;
	VkRenderPass renderPass;

	const VulkanState& mVulkanState;
	BufferInfo mCommonBufferInfo;
	BufferInfo mCommonStagingBufferInfo;
	BufferInfo mVertexBufferDesc, mIndexBufferDesc, mUniformBufferDesc, mUniformStagingBufferDesc;
	VulkanImageDesc mTextureDesc;
	VkDescriptorSetLayout mVkDescriptorSetLayout;
	VkSampler mTextureSampler;
	
	VkPipelineLayout mVkPipelineLayout;
	VkDescriptorPool mVkDescriptorPool;
	VkDescriptorSet mVkDescriptorSet;

private:
	void createBuffers();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffer();

	void createDescriptorSetLayout();
	void createPipeline();

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createDescriptorPool();
	void createDescriptorSet();
	void createRenderPass(const ImageHelper& vic);

	std::array<VkVertexInputAttributeDescription, 3> getAttrDesc() const;
	VkVertexInputBindingDescription getBindingDesc() const;

};

#endif


