#include "engine.h"

Engine::Engine()
{
	
}

Engine::~Engine() 
{

}

void Engine::init() 
{
	mWindow.initWindow(*this);
	mVulkanManager.createVkInstance();
	mVulkanManager.createVkSurface(*mWindow.mGlfwWindow);
	mVulkanManager.createPhysicalDevice();
	#if defined(AMVK_DBG)
	LOG("DBG ENABLED");
	mVulkanManager.enableDebug();
	#endif

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
