#include "quad.h"

Quad::Quad(VulkanState& vulkanState):
	mVulkanState(vulkanState), 
	mCommonBufferInfo(vulkanState.device),
	mCommonStagingBufferInfo(vulkanState.device),
	mVertexBufferDesc(vulkanState.device),
	mIndexBufferDesc(vulkanState.device),
	mUniformBufferDesc(vulkanState.device),
	mUniformStagingBufferDesc(vulkanState.device)
	//mTextureDesc(vulkanState.device)
{
	
}

Quad::~Quad() 
{
	mTextureDesc = nullptr;
}

void Quad::init()
{
	//createDescriptorSetLayout();
	//createPipeline();
	TextureDesc textureDesc(FileManager::getResourcePath("texture/statue.jpg"));//"texture/statue.jpg"));
	mTextureDesc = TextureManager::load(
			mVulkanState, 
			mVulkanState.commandPool, 
			mVulkanState.graphicsQueue, 
			textureDesc);

	createBuffers();

	//createDescriptorPool();
	createDescriptorSet();
}

void Quad::updateUniformBuffers(const Timer& timer, Camera& camera) 
{
	UBO ubo = {};
	ubo.model = glm::mat4();
	ubo.view = camera.view();
	ubo.proj = camera.proj();
	/*
	Buffer update via copy
	BufferHelper::mapMemory(mVulkanState, mCommonStagingBufferInfo.memory, mUniformBufferOffset, sizeof(ubo), &ubo);
	BufferHelper::copyBuffer(
			mVulkanState.device,
			mVulkanState.commandPool,
			mVulkanState.graphicsQueue,
			mCommonStagingBufferInfo.buffer, 
			mCommonBufferInfo.buffer,
			mUniformBufferOffset,
			sizeof(ubo));
	*/

	CmdPass cmdPass(mVulkanState.device, mVulkanState.commandPool, mVulkanState.graphicsQueue); 

	vkCmdUpdateBuffer(
			cmdPass.buffer,
			mCommonBufferInfo.buffer,
			mUniformBufferOffset,
			UBO_SIZE,
			&ubo);
	/*
	void* data;
	vkMapMemory(mVulkanState.device, mUniformStagingBufferDesc.memory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mVulkanState.device, mUniformStagingBufferDesc.memory);

	BufferHelper::copyBuffer(
			mVulkanState,
			mUniformStagingBufferDesc.buffer, 
			mUniformBufferDesc.buffer, 
			sizeof(ubo));
	*/
}

void Quad::update(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera) 
{
	//glm::rotate(glm::mat4(), (float) (10.f * timer.total() * glm::radians(90.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
	updatePushConstants(commandBuffer, timer, camera);
	//updateUniformBuffers(timer, camera);
} 

void Quad::updatePushConstants(VkCommandBuffer& commandBuffer, const Timer& timer, Camera& camera) 
{
	PushConstants pushConstants;
	pushConstants.model = glm::mat4();
	pushConstants.view = camera.view();
	pushConstants.proj = camera.proj();
	vkCmdPushConstants(
			commandBuffer, 
			mVulkanState.pipelines.quad.layout, 
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			sizeof(PushConstants),
			&pushConstants);
}

void Quad::draw(VkCommandBuffer& commandBuffer) 
{
	VkDeviceSize offset = mVertexBufferOffset;
	VkBuffer& commonBuff = mCommonBufferInfo.buffer;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &commonBuff, &offset);
	vkCmdBindIndexBuffer(commandBuffer, mCommonBufferInfo.buffer, mIndexBufferOffset, VK_INDEX_TYPE_UINT32);

	//vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertBuf, offsets);
	//vkCmdBindIndexBuffer(commandBuffer, mIndexBufferDesc.buffer, 0, VK_INDEX_TYPE_UINT32);
	
	vkCmdBindDescriptorSets(
			commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			mVulkanState.pipelines.quad.layout, 
			0, 
			1, 
			&mVkDescriptorSet, 
			0, 
			nullptr);

	vkCmdDrawIndexed(commandBuffer, numIndices, 1, 0, 0, 0);
}

void Quad::createBuffers() 
{
	// Vertex

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

	VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

	// Index

	const std::vector<uint32_t> indices = {
	    0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
	
	numIndices = indices.size(); 
	VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
	
	// Uniform
	//TODO: check how to add uniform without recopying same buffer
	VkDeviceSize uniformBufferSize = UBO_SIZE;
	UBO ubo = {};
	
	mUniformBufferOffset = 0;
	mVertexBufferOffset = uniformBufferSize;
	mIndexBufferOffset = uniformBufferSize + vertexBufferSize;

	//mVertexBufferOffset = 0;
	//mIndexBufferOffset = vertexBufferSize;
	//mUniformBufferOffset = vertexBufferSize + indexBufferSize;
	mCommonBufferInfo.size = vertexBufferSize + indexBufferSize + uniformBufferSize;
	mCommonStagingBufferInfo.size = mCommonBufferInfo.size;
	BufferHelper::createStagingBuffer(mVulkanState, mCommonStagingBufferInfo);
	
	char* data;
	vkMapMemory(mVulkanState.device, mCommonStagingBufferInfo.memory, 0, mCommonStagingBufferInfo.size, 0, (void**) &data);
	memcpy(data + mUniformBufferOffset, &ubo, (size_t) uniformBufferSize);
	memcpy(data + mVertexBufferOffset, vertices.data(), vertexBufferSize);
	memcpy(data + mIndexBufferOffset, indices.data(), indexBufferSize);
	vkUnmapMemory(mVulkanState.device, mCommonStagingBufferInfo.memory);

	BufferHelper::createCommonBuffer(mVulkanState, mCommonBufferInfo);

	BufferHelper::copyBuffer(
			mVulkanState,
			mCommonStagingBufferInfo.buffer, 
			mCommonBufferInfo.buffer, 
			mCommonBufferInfo.size);
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
	VkDeviceSize bufSize = UBO_SIZE;

	mUniformStagingBufferDesc.size = bufSize;
	mUniformBufferDesc.size = bufSize;

	BufferHelper::createStagingBuffer(mVulkanState, mUniformStagingBufferDesc);
	BufferHelper::createUniformBuffer(mVulkanState, mUniformBufferDesc);
}



VkVertexInputBindingDescription Quad::getBindingDesc() const
{
	VkVertexInputBindingDescription bindDesc = {};
	bindDesc.binding = 0;
	bindDesc.stride = VERTEX_SIZE;
	bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindDesc;
}
/*
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
}*/

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
	VkDescriptorSetLayout layouts[] = { mVulkanState.descriptorSetLayouts.quad };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = mVulkanState.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanState.device, &allocInfo, &mVkDescriptorSet));

	VkDescriptorBufferInfo buffInfo = {};
	buffInfo.buffer = mCommonBufferInfo.buffer;
	buffInfo.offset = mUniformBufferOffset;
	buffInfo.range = UBO_SIZE;

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mTextureDesc->imageView;
	imageInfo.sampler = mTextureDesc->sampler;

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


/*
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
}*/

/*
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
	
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {{descSetBinding, samplerLayoutBinding}};

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = bindings.size();
	descSetLayoutInfo.pBindings = bindings.data();

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanState.device, &descSetLayoutInfo, nullptr, &mVkDescriptorSetLayout));
	LOG("DESC LAYOUT CREATED");
}*/


void Quad::createPipeline(VulkanState& state)
{
	VkPipelineShaderStageCreateInfo stages[] = {
		state.shaders.quad.vertex,
		state.shaders.quad.fragment
	};

	VkVertexInputBindingDescription bindingDesc = {};
	bindingDesc.binding = 0;
	bindingDesc.stride = VERTEX_SIZE;
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//location, binding, format, offset
	VkVertexInputAttributeDescription attrDesc[] = { 
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) }
	};

	auto vertexInputInfo = PipelineCreator::vertexInputState(&bindingDesc, 1, attrDesc, ARRAY_SIZE(attrDesc)); 

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
	
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(state.physicalDevice, &physicalDeviceProperties);

	VkPushConstantRange pushConstantRange = PipelineCreator::pushConstantRange(
			state,
			VK_SHADER_STAGE_VERTEX_BIT, 
			0,
			PUSH_CONST_SIZE);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = PipelineCreator::layout(&state.descriptorSetLayouts.quad, 1, &pushConstantRange, 1);
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo;
	
	auto cache = FileManager::getInstance().readCache("quad");
	PipelineCreator::pipelineCache(state.device, cache, pipelineCacheCreateInfo);

	VK_CHECK_RESULT(vkCreatePipelineCache(state.device, &pipelineCacheCreateInfo, nullptr, &state.pipelines.quad.cache));
	VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &state.pipelines.quad.layout));

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
	pipelineInfo.layout = state.pipelines.quad.layout;
	pipelineInfo.renderPass = state.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &state.pipelines.quad.pipeline));
	
	if (cache.size() == 0) {
		LOG("CREATE CACHE");
		size_t cacheSize;
		VK_CHECK_RESULT(vkGetPipelineCacheData(state.device, state.pipelines.quad.cache, &cacheSize, NULL));
		char* cacheData = (char*) malloc(cacheSize);
		VK_CHECK_RESULT(vkGetPipelineCacheData(state.device, state.pipelines.quad.cache, &cacheSize, cacheData));
		LOG("CACHE SIZE: " << cacheSize << " DATA: " << (const char*) cacheData);

		FileManager::getInstance().writeCache("quad", cacheData, cacheSize);
		free(cacheData);
	}

}


