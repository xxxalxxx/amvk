#include "vulkan_manager.h"


VulkanManager::VulkanManager(Window& window):
	mWindow(window),
	mDeviceManager(mState),
	mSwapChainManager(mState, mWindow),
	tquad(mState),
	suit(mState),
    guard(mState),
	dwarf(mState),
	fullscreenQuad(mState),
	sceneLights(mState),
	imageIndex(0)
{
	
}

VulkanManager::~VulkanManager()
{
}

void VulkanManager::init() 
{
	mDeviceManager.createVkInstance();
#ifdef AMVK_DEBUG
	mDeviceManager.enableDebug();
#endif
	mSwapChainManager.createSurface();
	mDeviceManager.createPhysicalDevice(mSwapChainManager);
	mDeviceManager.createLogicalDevice();

	mSwapChainManager.createSwapChain();
	mSwapChainManager.createImageViews();
	
	mSwapChainManager.createRenderPass();
	mSwapChainManager.createCommandPool();

	ShaderManager::createShaders(mState);
	DescriptorManager::createDescriptorSetLayouts(mState);
	DescriptorManager::createDescriptorPool(mState);
    PipelineManager::createPipelines(mState, gBuffer);
	gBuffer.init(mState.physicalDevice, mState.device, mSwapChainManager.getWidth(), mSwapChainManager.getHeight());
	gBuffer.createCmdBuffer(mState.device, mState.commandPool);

	tquad.init();
	fullscreenQuad.init();

    suit.init(FileManager::getModelsPath("nanosuit/nanosuit.obj"),
              Model::DEFAULT_FLAGS | aiProcess_FlipUVs);

	dwarf.init(FileManager::getModelsPath("dwarf/dwarf2.ms3d"),
               Skinned::DEFAULT_FLAGS | aiProcess_FlipUVs | aiProcess_FlipWindingOrder,
               Skinned::ModelFlag_stripFullPath);

    dwarf.ubo.model = glm::scale(glm::vec3(0.15f, 0.15f, 0.15f));
    dwarf.ubo.model = glm::rotate(glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f)) * dwarf.ubo.model;
    dwarf.ubo.model = glm::rotate(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)) * dwarf.ubo.model;
    dwarf.ubo.model = glm::translate(glm::vec3(2.0f, 4.0f, 8.0f))  * dwarf.ubo.model;
    dwarf.animSpeedScale = 0.5f;

	guard.init(FileManager::getModelsPath("guard/boblampclean.md5mesh"),
               Skinned::DEFAULT_FLAGS | aiProcess_FlipUVs | aiProcess_FlipWindingOrder,
               0);
    guard.ubo.model = glm::scale(glm::vec3(0.18f, 0.18f, 0.18f));
    guard.ubo.model = glm::rotate(glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f)) * guard.ubo.model;
    guard.ubo.model = glm::rotate(glm::radians(-30.f), glm::vec3(0.f, 1.f, 0.f)) * guard.ubo.model;
    guard.ubo.model = glm::translate(glm::vec3(-9.0f, 4.0f, 8.0f))  * guard.ubo.model;

	sceneLights.init();

	mSwapChainManager.createDepthResources();
	mSwapChainManager.createFramebuffers(mState.renderPass);

	mSwapChainManager.createCommandBuffers();
	mSwapChainManager.createSemaphores();
	
	LOG("INIT SUCCESSFUL");
}


void VulkanManager::updateUniformBuffers(const Timer& timer, Camera& camera)
{
	CmdPass cmd(mState.device, mState.commandPool, mState.graphicsQueue);
	tquad.update(cmd.buffer, timer, camera);
    suit.update(cmd.buffer, timer, camera);
	dwarf.update(cmd.buffer, timer, camera);
	guard.update(cmd.buffer, timer, camera);
	sceneLights.update(cmd.buffer, timer, camera);
}

void VulkanManager::buildGBuffers(const Timer &timer, Camera &camera) 
{
	std::array<VkClearValue, GBuffer::ATTACHMENT_COUNT> clearValues;
	clearValues[GBuffer::INDEX_POSITION].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
	clearValues[GBuffer::INDEX_NORMAL].color   = { { 0.0f, 0.0f, 0.0f, 0.0f } };
	clearValues[GBuffer::INDEX_ALBEDO].color   = { { 0.0f, 0.0f, 0.0f, 0.0f } };
	clearValues[GBuffer::INDEX_DEPTH].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.framebuffer = gBuffer.frameBuffer;
	renderPassBeginInfo.renderPass = gBuffer.renderPass;
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent.width = gBuffer.width;
	renderPassBeginInfo.renderArea.extent.height = gBuffer.height;
	renderPassBeginInfo.clearValueCount = clearValues.size();
	renderPassBeginInfo.pClearValues = clearValues.data();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkCommandBuffer& cmdBuffer = gBuffer.cmdBuffer;

	VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

	vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


	VkViewport viewport = {};
	viewport.width = (float) gBuffer.width;
	viewport.height = (float) gBuffer.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent.width = gBuffer.width;
	scissor.extent.height = gBuffer.height;

	vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

	dwarf.draw(cmdBuffer, mState.pipelines.skinned.pipeline, mState.pipelines.skinned.layout);
	suit.draw(cmdBuffer, mState.pipelines.model.pipeline, mState.pipelines.model.layout);

	vkCmdEndRenderPass(cmdBuffer);
	VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
}

void VulkanManager::buildComputeBuffers(const Timer &timer, Camera &camera) 
{

}

void VulkanManager::buildCommandBuffers(const Timer &timer, Camera &camera)
{

	VkClearValue clearValues[] ={
		{{0.4f, 0.1f, 0.1f, 1.0f}},	// VkClearColorValue color; 
		{{1.0f, 0}} // VkClearDepthStencilValue depthStencil 
	};


	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = mState.renderPass;
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = mState.swapChainExtent;
	renderPassBeginInfo.clearValueCount = ARRAY_SIZE(clearValues);
	renderPassBeginInfo.pClearValues = clearValues;
	
	for (size_t i = 0; i < mSwapChainManager.cmdBuffers.size(); ++i) {
        VkCommandBuffer& cmdBuffer = mSwapChainManager.cmdBuffers[i];
		VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
		renderPassBeginInfo.framebuffer = mSwapChainManager.framebuffers[i];
		vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) mState.swapChainExtent.width;
		viewport.height = (float) mState.swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = mState.swapChainExtent;

		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);


		//tquad.draw(cmdBuffer);
		 //fullscreenQuad.draw(cmdBuffer);
		//sceneLights.draw(cmdBuffer);
		dwarf.draw(cmdBuffer, mState.pipelines.skinned.pipeline, mState.pipelines.skinned.layout);
		suit.draw(cmdBuffer, mState.pipelines.model.pipeline, mState.pipelines.model.layout);
	
        //guard.draw(cmdBuffer, mState.pipelines.skinned.pipeline, mState.pipelines.skinned.layout);
		vkCmdEndRenderPass(cmdBuffer);
		VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
	}
}

void VulkanManager::draw() 
{
	VkResult result = vkAcquireNextImageKHR(mState.device,
                                            mState.swapChain,
										  std::numeric_limits<uint64_t>::max(), 
										  mSwapChainManager.mImageAvailableSemaphore, 
										  VK_NULL_HANDLE, 
										  &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
	} else if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		
		VkSemaphore waitSemaphores[] = { mSwapChainManager.mImageAvailableSemaphore };
		VkSemaphore signalSemaphores[] = { mSwapChainManager.mRenderFinishedSemaphore };
		VkSwapchainKHR swapChains[] = { mState.swapChain };
		VkPipelineStageFlags stageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = stageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mSwapChainManager.cmdBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK_RESULT(vkQueueSubmit(mState.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
		
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		VK_CHECK_RESULT(vkQueuePresentKHR(mState.presentQueue, &presentInfo));
	} else {
		VK_THROW_RESULT_ERROR("Failed vkAcquireNextImageKHR", result);
	}
}

void VulkanManager::waitIdle() 
{
	vkDeviceWaitIdle(mState.device);
}

void VulkanManager::recreateSwapChain()
{
/*	vkQueueWaitIdle(mState.graphicsQueue);
	vkDeviceWaitIdle(mState.device);

	vkFreeCommandBuffers(mState.device, mState.commandPool, (uint32_t) mVkCommandBuffers.size(), mVkCommandBuffers.data());
	
	for (auto& framebuffer : mSwapChainFramebuffers)
		vkDestroyFramebuffer(mState.device, framebuffer, nullptr);

	vkDestroyImageView(mState.device, mDepthImageView, nullptr);
	vkDestroyImage(mState.device, mDepthImage, nullptr);
	vkFreeMemory(mState.device, mDepthImageMem, nullptr);
	
	vkDestroyPipeline(mState.device, mVkPipeline, nullptr);
	vkDestroyRenderPass(mState.device, mState.renderPass, nullptr);

	for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
		vkDestroyImageView(mState.device, mSwapC	}

	vkDestroySwapchainKHR(mState.device, mState.swapChain, nullptr);

	createSwapChain(mWindow);
    createImageViews();
    createRenderPass();
    createPipeline();
	createDepthResources();
    createFramebuffers();
    createCommandBuffers();*/
}


