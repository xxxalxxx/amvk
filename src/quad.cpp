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

	ImageHelper vic(mVulkanState);

	//createRenderPass(vic);

	createDescriptorSetLayout();
	createPipeline();

	createTextureImage(vic);
	createTextureImageView(vic);
	createTextureSampler();

	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();

	createDescriptorPool();
	createDescriptorSet();
}

void Quad::updateUniformBuffers() 
{
	UBO ubo = {};
	
	void* data;
	vkMapMemory(mVulkanState.device, mUniformStagingBufferDesc.memory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mVulkanState.device, mUniformStagingBufferDesc.memory);

	BufferHelper::copyBuffer(
			mVulkanState,	
			mUniformStagingBufferDesc.buffer, 
			mUniformBufferDesc.buffer, 
			sizeof(ubo));

}

void Quad::update(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera) 
{
	PushConstants pushConstants;
	pushConstants.model = glm::mat4();//glm::rotate(glm::mat4(), (float) (10.f * timer.total() * glm::radians(90.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
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

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertBuf, offsets);
	vkCmdBindIndexBuffer(commandBuffer, mIndexBufferDesc.buffer, 0, VK_INDEX_TYPE_UINT32);
	
	vkCmdBindDescriptorSets(
			commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			mVkPipelineLayout, 
			0, 
			1, 
			&mVkDescriptorSet, 
			0, 
			nullptr);

	vkCmdDrawIndexed(commandBuffer, numIndices, 1, 0, 0, 0);
}

void Quad::createVertexBuffer() 
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
	mVertexBufferDesc.size = bufferSize;
	BufferInfo stagingDesc(mVulkanState.device, bufferSize);

	BufferHelper::createStagingBuffer(mVulkanState,stagingDesc);
	BufferHelper::mapMemory(mVulkanState, stagingDesc, vertices.data());
	BufferHelper::createVertexBuffer(mVulkanState, mVertexBufferDesc);

	BufferHelper::copyBuffer(
			mVulkanState,
			stagingDesc.buffer, 
			mVertexBufferDesc.buffer, 
			bufferSize);
}

void Quad::createIndexBuffer()
{
	const std::vector<uint32_t> indices = {
	    0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
	
	numIndices = indices.size(); 
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	mIndexBufferDesc.size = bufferSize;
	BufferInfo stagingDesc(mVulkanState.device, bufferSize);

	BufferHelper::createStagingBuffer(mVulkanState, stagingDesc);
	BufferHelper::mapMemory(mVulkanState, stagingDesc, indices.data());
	BufferHelper::createIndexBuffer(mVulkanState,mIndexBufferDesc);
	
	BufferHelper::copyBuffer(
			mVulkanState,
			stagingDesc.buffer, 
			mIndexBufferDesc.buffer, 
			bufferSize);
}

void Quad::createUniformBuffer()
{	
	VkDeviceSize bufSize = sizeof(UBO);

	mUniformStagingBufferDesc.size = bufSize;
	mUniformBufferDesc.size = bufSize;

	BufferHelper::createStagingBuffer(mVulkanState, mUniformStagingBufferDesc);
	BufferHelper::createUniformBuffer(mVulkanState, mUniformBufferDesc);
}

void Quad::createTextureImage(const ImageHelper& vic)
{
	TextureData textureData;
	textureData.load("texture/statue.jpg", STBI_rgb_alpha);
	
	int w = textureData.getWidth();
	int h = textureData.getHeight(); 
	VkDeviceSize imageSize = textureData.getSize();
	stbi_uc* pixels = textureData.getPixels(); 

	mTextureDesc.width = w;
	mTextureDesc.height = h;
	
	VulkanImageDesc stagingDesc(mVulkanState.device, w, h);

	ImageHelper::createImage(
			mVulkanState, 
			stagingDesc, 
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_LINEAR, 
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); 
	
	BufferHelper::mapMemory(mVulkanState, stagingDesc.memory, imageSize, pixels);

	ImageHelper::createImage(
			mVulkanState, 
			mTextureDesc, 
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	ImageHelper::transitionLayout(
			mVulkanState,
			stagingDesc.image,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_PREINITIALIZED, 
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_ACCESS_HOST_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT);

	ImageHelper::transitionLayout(
			mVulkanState,
			mTextureDesc.image,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_LAYOUT_PREINITIALIZED, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_ACCESS_HOST_WRITE_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT);

	ImageHelper::copyImage(mVulkanState, stagingDesc, mTextureDesc);

	ImageHelper::transitionLayout(
			mVulkanState,
			mTextureDesc.image,
			VK_FORMAT_R8G8B8A8_UNORM, 
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT);
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
	VkPipelineShaderStageCreateInfo vertStageCreateInfo = PipelineCreator::shaderStage(mVulkanState.device, "shader.vert", VK_SHADER_STAGE_VERTEX_BIT);
	VkPipelineShaderStageCreateInfo fragStageCreateInfo = PipelineCreator::shaderStage(mVulkanState.device, "shader.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo stages[] = {
		vertStageCreateInfo,
		fragStageCreateInfo
	};

	auto bindingDesc = getBindingDesc();
	auto attrDesc = getAttrDesc();
	auto vertexInputInfo = getVertexInputStateCreateInfo(bindingDesc, attrDesc);

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo = PipelineCreator::inputAssemblyNoRestart(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	VkPipelineViewportStateCreateInfo viewportState = PipelineCreator::viewportStateDynamic();

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicInfo = PipelineCreator::dynamicState(dynamicStates, ARRAY_SIZE(dynamicStates));
	VkPipelineRasterizationStateCreateInfo rasterizationState = PipelineCreator::rasterizationStateCullBackCCW();
	VkPipelineDepthStencilStateCreateInfo depthStencil = PipelineCreator::depthStencilStateDepthLessNoStencil();
	VkPipelineMultisampleStateCreateInfo multisampleState = PipelineCreator::multisampleStateNoMultisampleNoSampleShading();
	VkPipelineColorBlendAttachmentState blendAttachmentState = PipelineCreator::blendAttachmentStateDisabled();

	VkPipelineColorBlendStateCreateInfo blendState = PipelineCreator::blendStateDisabled(&blendAttachmentState, 1); 

	VkDescriptorSetLayout setLayouts[] = { mVkDescriptorSetLayout };
	
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(mVulkanState.physicalDevice, &physicalDeviceProperties);

	VkPushConstantRange pushConstantRange = PipelineCreator::pushConstantRange(
			mVulkanState,
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			sizeof(PushConstants));

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(setLayouts, 1, &pushConstantRange, 1);

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
	pipelineInfo.renderPass = mVulkanState.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(mVulkanState.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mVkPipeline));
	LOG("PIPELINE CREATED");
}

void Quad::createTextureImageView(const ImageHelper& vic)
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

void Quad::createRenderPass(const ImageHelper& vic)
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


