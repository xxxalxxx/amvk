
#include <GLFW/glfw3.h>
#include <iostream>
#include "macro.h"
#include "task_manager.h"
#include "engine.h"

int main() {

	Engine engine;
	engine.init();
	
	Window& window = engine.getWindow();
	InputManager& inputManager = window.getInputManager();
	VulkanManager& vulkanManager = engine.getVulkanManager();
	Timer& timer = engine.getTimer();
	while (window.isOpen()) {
		inputManager.pollEvents();
		timer.tick();
		vulkanManager.updateUniformBuffer(timer);
		vulkanManager.updateCommandBuffers(timer);
		vulkanManager.draw();
	}
	vulkanManager.waitIdle();
	return 0;
}
