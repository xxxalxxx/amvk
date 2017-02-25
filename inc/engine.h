#ifndef AMVK_ENGINE_H
#define AMVK_ENGINE_H

#include <vector>

#include "macro.h"
#include "task_manager.h"
#include "vulkan_manager.h"
#include "window.h"
#include "timer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Engine {
public:
	Engine();
	virtual ~Engine();
	void init();
	void handleMovement(double dt);
	Window& getWindow();
	TaskManager& getTaskManager();
	VulkanManager& getVulkanManager();
	Timer& getTimer();
	Camera& getCamera();
private:
	Window mWindow;
	Camera mCamera;
	TaskManager mTaskManager;
	VulkanManager mVulkanManager;
	Timer mTimer;

};


#endif
