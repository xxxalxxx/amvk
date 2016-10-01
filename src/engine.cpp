#include "engine.h"

Engine::Engine():
	mVulkanManager(mWindow)
{
	
}

Engine::~Engine() 
{

}

void onWindowResized(GLFWwindow* window, int width, int height) {
	if (width * height == 0) 
		return;

//	Engine* eng = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
//	eng->getVulkanManager().recreateSwapChain();
}


void Engine::init() 
{
	mWindow.initWindow(*this);
	mWindow.setWindowSizeCallback(onWindowResized);
	mVulkanManager.createVkInstance();

	#ifdef AMVK_DBG
	LOG("DBG ENABLED");
	mVulkanManager.enableDebug();
	#endif

	mVulkanManager.createVkSurface(*(mWindow.mGlfwWindow));
	mVulkanManager.createPhysicalDevice();
	mVulkanManager.createLogicalDevice();
	mVulkanManager.createSwapChain(mWindow);
	mVulkanManager.createImageViews();
	mVulkanManager.createRenderPass();
	mVulkanManager.createPipeline();
	mVulkanManager.createCommandPool();
	mVulkanManager.createFramebuffers();
	mVulkanManager.createCommandBuffers();
	mVulkanManager.createSemaphores();
	LOG("INIT SUCCESSFUL");
}

Window& Engine::getWindow()
{
	return mWindow;
} 

TaskManager& Engine::getTaskManager() 
{
	return mTaskManager;
} 

VulkanManager& Engine::getVulkanManager()
{
	return mVulkanManager;
}
