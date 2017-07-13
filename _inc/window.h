#ifndef AMVK_WINDOW_H
#define AMVK_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <exception>
#include <stdexcept>

#include "macro.h"
#include "input_manager.h"
#include "camera.h"

class Engine;

class Window {
	friend class Engine;

public:
	Window();
	Window(uint32_t w, uint32_t h);
	virtual ~Window();
	inline bool isOpen() { return !glfwWindowShouldClose(mGlfwWindow); }
	InputManager& getInputManager();

	void setDimens(uint32_t w, uint32_t h);
	void setWidth(uint32_t w);
	void setHeight(uint32_t h);
	void setWindowSizeCallback(GLFWwindowsizefun f);
	
	unsigned getWidth() const;
	unsigned getHeight() const;
	float getAspect() const;

	GLFWwindow* mGlfwWindow;
private:
	void initWindow(Engine& engine);

	void calcAspect();

	InputManager mInputManager;

	unsigned mWidth, mHeight;
	float mAspect;

};

#endif
