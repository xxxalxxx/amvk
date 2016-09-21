#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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
		vulkanManager.draw();
	}
	vulkanManager.waitIdle();
	return 0;
}
