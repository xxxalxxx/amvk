#include "quad.h"

Quad::Quad(const VulkanState& vulkanState):
	mVulkanState(vulkanState), 
	mVertexBufferDesc(vulkanState.device),
	mIndexBufferDesc(vulkanState.device),
	mUniformBufferDesc(vulkanState.device),
	mUniformStagingBufferDesc(vulkanState.device),
	mTextureDesc(vulkanState.device)
{
	
}

Quad::~Quad() 
{

}

void Quad::init()
{
	VulkanBufferCreator vbc(mVulkanState);
	VulkanImageCreator vic(mVulkanState);



	createRenderPass(vic);

	createDescriptorSetLayout();
	createPipeline();

	createTextureImage(vic);
	createTextureImageView(vic);
	createTextureSampler();

	createVertexBuffer(vbc);
	createIndexBuffer(vbc);
	createUniformBuffer(vbc);

	createDescriptorPool();
	createDescriptorSet();
}

void Quad::update(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera)
{	
	PushConstants pushConstants;
	pushConstants.model = glm::rotate(glm::mat4(), (float) timer.total() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pushConstants.view = camera.view();//glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pushConstants.proj = camera.proj(); //glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float) mSwapChainExtent.height, 0.1f, 10.0f);

	vkCmdPushConstants(
			commandBuffer, 
			mVkPipelineLayout, 
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			sizeof(PushConstants),
			&pushConstants);

} 

void Quad::draw(VkCommandBuffer& commandBuffer) 
{

	VkBuffer vertBuf[] = {mVertexBufferDesc.buffer};
	VkDeviceSize offsets[] = {0};

	LOG("Q 1");
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertBuf, offsets);
	
	LOG("Q 2");
	vkCmdBindIndexBuffer(commandBuffer, mIndexBufferDesc.buffer, 0, VK_INDEX_TYPE_UINT32);
	
	LOG("Q 3");
	vkCmdBindDescriptorSets(
			commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			mVkPipelineLayout, 
			0, 
			1, 
			&mVkDescriptorSet, 
			0, 
			nullptr);

	LOG("Q 4");
	vkCmdDrawIndexed(commandBuffer, numIndices, 1, 0, 0, 0);
}

void Quad::createVertexBuffer(const VulkanBufferCreator& vbc) 
{
	const std::vector<Vertex> vertices = {
	    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VulkanBufferDesc stagingDesc(mVulkanState.device);

	vbc.createBuffer(
			stagingDesc.buffer, 
			bufferSize, 
			stagingDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(mVulkanState.device, stagingDesc.memory, 0, bufferSize, 0 , &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(mVulkanState.device, stagingDesc.memory);
	vbc.createBuffer(
			mVertexBufferDesc.buffer, 
			bufferSize, 
			mVertexBufferDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vbc.copyBuffer(stagingDesc.buffer, mVertexBufferDesc.buffer, bufferSize);

	//vkDestroyBuffer(mVulkanState.device, stagingDesc.buffer, nullptr);
	//vkFreeMemory(mVulkanState.device, stagingDesc.memory, nullptr);
}

void Quad::createIndexBuffer(const VulkanBufferCreator& vbc)
{
	const std::vector<uint32_t> indices = {
	    0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
	
	numIndices = indices.size(); 
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VulkanBufferDesc stagingDesc(mVulkanState.device);

	vbc.createBuffer(
			stagingDesc.buffer, 
			bufferSize, 
			stagingDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(mVulkanState.device, stagingDesc.memory, 0, bufferSize, 0 , &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(mVulkanState.device, stagingDesc.memory);
	vbc.createBuffer(
			mIndexBufferDesc.buffer, 
			bufferSize, 
			mIndexBufferDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	
	vbc.copyBuffer(stagingDesc.buffer, mIndexBufferDesc.buffer, bufferSize);
}

void Quad::createUniformBuffer(const VulkanBufferCreator& vbc)
{	
	VkDeviceSize bufSize = sizeof(UBO);
	vbc.createBuffer(
			mUniformStagingBufferDesc.buffer, 
			bufSize, 
			mUniformStagingBufferDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	vbc.createBuffer(
			mUniformBufferDesc.buffer, 
			bufSize, 
			mUniformBufferDesc.memory, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void Quad::createTextureImage(const VulkanImageCreator& vic)
{
	TextureData textureData;
	textureData.load("texture/statue.jpg", STBI_rgb_alpha);
	
	int w = textureData.getWidth();
	int h = textureData.getHeight(); 
	VkDeviceSize imageSize = textureData.getSize();
	stbi_uc* pixels = textureData.getPixels(); 

	VulkanImageDesc stagingDesc(mVulkanState.device);

	vic.createImage(
			w, 
			h, 
			VK_FORMAT_R8G8B8A8_UNORM, 
			VK_IMAGE_TILING_LINEAR,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingDesc.image, 
			stagingDesc.memory);

	void* data;
	vkMapMemory(mVulkanState.device, stagingDesc.memory, 0, imageSize, 0, &data);
	memcpy(data, pixels, (size_t) imageSize);
	vkUnmapMemory(mVulkanState.device, stagingDesc.memory);

    vic.createImage(
			w, 
			h, 
			VK_FORMAT_R8G8B8A8_UNORM, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			mTextureDesc.image, 
			mTextureDesc.memory);

	vic.transitionImageLayout(
			stagingDesc.image,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_PREINITIALIZED, 
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	vic.transitionImageLayout(
			mTextureDesc.image, 
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_PREINITIALIZED, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	
	vic.copyImage(stagingDesc.image, mTextureDesc.image, w, h);

	vic.transitionImageLayout(
			mTextureDesc.image, 
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkVertexInputBindingDescription Quad::getBindingDesc() const
{
	VkVertexInputBindingDescription bindDesc = {};
	bindDesc.binding = 0;
	bindDesc.stride = sizeof(Vertex);
	bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindDesc;
}

void Quad::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	VK_CHECK_RESULT(vkCreateDescriptorPool(mVulkanState.device, &poolInfo, nullptr, &mVkDescriptorPool));
}

VkPipelineVertexInputStateCreateInfo Quad::getVertexInputStateCreateInfo(
		VkVertexInputBindingDescription& bindingDesc, 
		std::array<VkVertexInputAttributeDescription, 3>& attrDesc) const
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = attrDesc.size();
	vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();

	return vertexInputInfo;
}

std::array<VkVertexInputAttributeDescription, 3> Quad::getAttrDesc() const 
{
    // uint32_t    location;
    // uint32_t    binding;
    // VkFormat    format;
    // uint32_t    offset;

	std::array<VkVertexInputAttributeDescription, 3> attrDesc = {{
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) }
	}};

	return attrDesc;
} 

void Quad::createDescriptorSet() 
{
	VkDescriptorSetLayout layouts[] = { mVkDescriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = mVkDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanState.device, &allocInfo, &mVkDescriptorSet));

	VkDescriptorBufferInfo buffInfo = {};
	buffInfo.buffer = mUniformBufferDesc.buffer;
	buffInfo.offset = 0;
	buffInfo.range = sizeof(UBO);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mTextureDesc.imageView;
	imageInfo.sampler = mTextureSampler;

	std::array<VkWriteDescriptorSet, 2> writeSets = {};

	writeSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSets[0].dstSet = mVkDescriptorSet;
	writeSets[0].dstBinding = 0;
	writeSets[0].dstArrayElement = 0;
	writeSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSets[0].descriptorCount = 1;
	writeSets[0].pBufferInfo = &buffInfo;

	writeSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSets[1].dstSet = mVkDescriptorSet;
	writeSets[1].dstBinding = 1;
	writeSets[1].dstArrayElement = 0;
	writeSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeSets[1].descriptorCount = 1;
	writeSets[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(mVulkanState.device, writeSets.size(), writeSets.data(), 0, nullptr);
}



void Quad::createPipeline()
{
	VulkanPipelineCreator pc;
	
	// FileManager& fileManager = FileManager::getInstance();
	// auto vertShaderSrc = fileManager.readShader("shader.vert");
	// auto fragShaderSrc = fileManager.readShader("shader.frag");
	// createShaderModule(vertShaderSrc, vertShaderModule);
	// createShaderModule(fragShaderSrc, fragShaderModule);

	VkPipelineShaderStageCreateInfo vertStageCreateInfo = pc.shaderStage(mVulkanState.device, "shader.vert", VK_SHADER_STAGE_VERTEX_BIT);
	VkPipelineShaderStageCreateInfo fragStageCreateInfo = pc.shaderStage(mVulkanState.device, "shader.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo stages[] = {
		vertStageCreateInfo,
		fragStageCreateInfo
	};

	auto bindingDesc = getBindingDesc();
	auto attrDesc = getAttrDesc();
	auto vertexInputInfo = getVertexInputStateCreateInfo(bindingDesc, attrDesc);

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo = pc.inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	VkPipelineViewportStateCreateInfo viewportState = pc.viewportStateDynamic();

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicInfo = pc.dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
	VkPipelineRasterizationStateCreateInfo rasterizationState = pc.rasterizationStateCullBackCCW();
	VkPipelineDepthStencilStateCreateInfo depthStencil = pc.depthStencilStateDepthLessNoStencil();
	VkPipelineMultisampleStateCreateInfo multisampleState = pc.multisampleStateNoMultisampleNoSampleShading();
	VkPipelineColorBlendAttachmentState blendAttachmentState = pc.blendAttachmentStateDisabled();

	VkPipelineColorBlendStateCreateInfo blendState = pc.blendStateDisabled(&blendAttachmentState, 1); 

	VkDescriptorSetLayout setLayouts[] = { mVkDescriptorSetLayout };
	
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(mVulkanState.physicalDevice, &physicalDeviceProperties);
	uint32_t maxPushConstantsSize = physicalDeviceProperties.limits.maxPushConstantsSize;
	LOG("MAX PUSH CONST SIZE max:" << maxPushConstantsSize << " curr:" << sizeof(PushConstants));

	if (sizeof(PushConstants) > maxPushConstantsSize)
		throw std::runtime_error("Push Constants exceed max size. Use uniform buffer.");
	if (sizeof(PushConstants) % 4 != 0)
		throw std::runtime_error("Push Constants size must be a multiple of 4");

	VkPushConstantRange pushConstantRange;
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstants); 

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = pc.layout(setLayouts, 1, &pushConstantRange, 1);

	VK_CHECK_RESULT(vkCreatePipelineLayout(mVulkanState.device, &pipelineLayoutInfo, nullptr, &mVkPipelineLayout));

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = ARRAY_SIZE(stages);
	pipelineInfo.pStages = stages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &assemblyInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizationState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &blendState;
	pipelineInfo.pDynamicState = &dynamicInfo;
	pipelineInfo.layout = mVkPipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(mVulkanState.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline));
	LOG("PIPELINE CREATED");
}

void Quad::createTextureImageView(const VulkanImageCreator& vic)
{
	vic.createImageView(mTextureDesc.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mTextureDesc.imageView);
}

void Quad::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	VK_CHECK_RESULT(vkCreateSampler(mVulkanState.device, &samplerInfo, nullptr, &mTextureSampler));
}

void Quad::createRenderPass(const VulkanImageCreator& vic)
{
	VulkanRenderPassCreator renderPassCreator;

	VkAttachmentDescription att = renderPassCreator.attachmentDescColor(mVulkanState.swapChainImageFormat);
	VkAttachmentDescription depthAtt = renderPassCreator.attachmentDescDepthNoStencil(vic.findDepthFormat());

	VkAttachmentReference attRef = {};
	attRef.attachment = 0;
	attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttRef = {};
	depthAttRef.attachment = 1;
	depthAttRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription sub = {};
	sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	sub.colorAttachmentCount = 1;
	sub.pColorAttachments = &attRef;
	sub.pDepthStencilAttachment = &depthAttRef;

	VkSubpassDependency dependancy = {};
	dependancy.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependancy.dstSubpass = 0;
	dependancy.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependancy.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependancy.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependancy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT 
							 | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = {
		att, 
		depthAtt
	};
	
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = attachments.size();
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &sub;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependancy;

	VK_CHECK_RESULT(vkCreateRenderPass(mVulkanState.device, &createInfo, nullptr, &renderPass));

	LOG("RENDER PASS CREATED");
}

void Quad::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descSetBinding = {};
	descSetBinding.binding = 0;
	descSetBinding.descriptorCount = 1;
	descSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descSetBinding.pImmutableSamplers = nullptr;
	descSetBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {descSetBinding, samplerLayoutBinding};

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = bindings.size();
	descSetLayoutInfo.pBindings = bindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanState.device, &descSetLayoutInfo, nullptr, &mVkDescriptorSetLayout));
	LOG("DESC LAYOUT CREATED");
}


