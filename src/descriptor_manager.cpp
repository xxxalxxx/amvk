#include "descriptor_manager.h"

DescriptorManager::DescriptorManager(VulkanState& state):
	mState(state), 
	mLayouts(state.descriptorSetLayouts)
	{}

void DescriptorManager::createDescriptorSetLayouts()
{
	createQuadDescriptorSetLayout();
	createModelDescriptorSetLayout();
	createSamplerDescriptorSetLayout();
	createUniformDescriptorSetLayout();
	LOG("DESC LAYOUTS CREATED");
}

void DescriptorManager::createQuadDescriptorSetLayout()
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
	
	VkDescriptorSetLayoutBinding bindings[] = {
		descSetBinding, 
		samplerLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = ARRAY_SIZE(bindings);
	descSetLayoutInfo.pBindings = bindings;

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mState.device, &descSetLayoutInfo, nullptr, &mLayouts.quad));
}


void DescriptorManager::createSamplerDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = 1; 
	descSetLayoutInfo.pBindings = &samplerLayoutBinding;

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mState.device, &descSetLayoutInfo, nullptr, &mLayouts.sampler));
}

void DescriptorManager::createUniformDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descSetBinding = {};
	descSetBinding.binding = 0;
	descSetBinding.descriptorCount = 1;
	descSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descSetBinding.pImmutableSamplers = nullptr;
	descSetBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = 1;
	descSetLayoutInfo.pBindings = &descSetBinding;

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mState.device, &descSetLayoutInfo, nullptr, &mLayouts.uniform));
}

void DescriptorManager::createModelDescriptorSetLayout()
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

	VkDescriptorSetLayoutBinding samplerSpecLayoutBinding = {};
	samplerSpecLayoutBinding.binding = 2;
	samplerSpecLayoutBinding.descriptorCount = 0;
	samplerSpecLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerSpecLayoutBinding.pImmutableSamplers = nullptr;
	samplerSpecLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerHeightLayoutBinding = {};
	samplerHeightLayoutBinding.binding = 3;
	samplerHeightLayoutBinding.descriptorCount = 0;
	samplerHeightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerHeightLayoutBinding.pImmutableSamplers = nullptr;
	samplerHeightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerAmbientLayoutBinding = {};
	samplerAmbientLayoutBinding.binding = 4;
	samplerAmbientLayoutBinding.descriptorCount = 0;
	samplerAmbientLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerAmbientLayoutBinding.pImmutableSamplers = nullptr;
	samplerAmbientLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] = {
		descSetBinding, 
		samplerLayoutBinding,
		samplerSpecLayoutBinding,
		samplerHeightLayoutBinding,
		samplerAmbientLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = ARRAY_SIZE(bindings);
	descSetLayoutInfo.pBindings = bindings;

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mState.device, &descSetLayoutInfo, nullptr, &mLayouts.model));

	LOG("MODEL DESC LAYOUT CREATED");
}

void DescriptorManager::createDescriptorPool() 
{
	VkDescriptorPoolSize uboSize = {};
	uboSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboSize.descriptorCount = 1;
	
	VkDescriptorPoolSize samplerSize = {};
	samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerSize.descriptorCount = 1;

	VkDescriptorPoolSize poolSizes[] = {
		uboSize,
		samplerSize
	};
	
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = ARRAY_SIZE(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 1;

	VK_CHECK_RESULT(vkCreateDescriptorPool(mState.device, &poolInfo, nullptr, &mState.descriptorPool));
}
