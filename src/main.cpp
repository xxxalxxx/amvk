
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
	Camera& camera = engine.getCamera();
	while (window.isOpen()) {
		inputManager.pollEvents();
		double dt = timer.tick();
		engine.handleMovement(dt);

		vulkanManager.updateUniformBuffer(timer);
		vulkanManager.updateCommandBuffers(timer, camera);

		vulkanManager.draw();
	}
	vulkanManager.waitIdle();
	return 0;
}
