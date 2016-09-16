#ifndef AMVK_ENGINE_H
#define AMVK_ENGINE_H

#include <vector>

#include "macro.h"
#include "window.h"
#include "task_manager.h"
#include "vulkan_manager.h"

class Engine {
public:
	Engine();
	virtual ~Engine();
	void init();

	Window& getWindow();
	TaskManager& getTaskManager();
	VulkanManager& getVulkanManager();

private:
	Window mWindow;
	TaskManager mTaskManager;
	VulkanManager mVulkanManager;
};


#endif
