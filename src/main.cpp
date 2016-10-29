
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

	while (window.isOpen()) {
		inputManager.pollEvents();
		vulkanManager.updateUniformBuffer();
		vulkanManager.updateCommandBuffers();
		vulkanManager.draw();
	}
	vulkanManager.waitIdle();
	return 0;
}
