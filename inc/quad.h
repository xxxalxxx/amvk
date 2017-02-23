#ifndef AMVK_QUAD_H
#define AMVK_QUAD_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstring>
#include <cstddef>
#include <string.h>
#include <array>
#include <vector>

#include "macro.h"
#include "vulkan_buffer_creator.h"
#include "vulkan_image_creator.h"
#include "vulkan_render_pass_creator.h"
#include "texture_manager.h"
#include "vulkan_pipeline_creator.h"
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
	void updateUniformBuffers(); 

	VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo(
		VkVertexInputBindingDescription& bindingDesc, 
		std::array<VkVertexInputAttributeDescription, 3>& attrDesc) const;

	uint32_t numIndices;
	VkPipeline mVkPipeline;
	VkRenderPass renderPass;


	const VulkanState& mVulkanState;
	VulkanBufferDesc mVertexBufferDesc, mIndexBufferDesc, mUniformBufferDesc, mUniformStagingBufferDesc;
	VulkanImageDesc mTextureDesc;
	VkDescriptorSetLayout mVkDescriptorSetLayout;
	VkSampler mTextureSampler;
	
	VkPipelineLayout mVkPipelineLayout;
	VkDescriptorPool mVkDescriptorPool;
	VkDescriptorSet mVkDescriptorSet;

private:

	void createVertexBuffer(const VulkanBufferCreator& vbc);
	void createIndexBuffer(const VulkanBufferCreator& vbc);
	void createUniformBuffer(const VulkanBufferCreator& vbc);

	void createDescriptorSetLayout();
	void createPipeline();

	void createTextureImage(const VulkanImageCreator& vic);
	void createTextureImageView(const VulkanImageCreator& vic);
	void createTextureSampler();
	void createDescriptorPool();
	void createDescriptorSet();
	void createRenderPass(const VulkanImageCreator& vic);

	std::array<VkVertexInputAttributeDescription, 3> getAttrDesc() const;
	VkVertexInputBindingDescription getBindingDesc() const;

};

#endif

