#ifndef AMVK_WINDOW_H
#define AMVK_WINDOW_H

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include <exception>
#include <stdexcept>

#include "macro.h"
#include "input_manager.h"

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

class Engine;

class Window {
	friend class Engine;
public:
	Window();
	Window(unsigned w, unsigned h);
	virtual ~Window();
	inline bool isOpen() { return !glfwWindowShouldClose(mGlfwWindow); }
	InputManager& getInputManager();

	void setDimens(unsigned w, unsigned h);
	void setWidth(unsigned w);
	void setHeight(unsigned h);
	
	unsigned getWidth() const;
	unsigned getHeight() const;
	float getAspect() const;
private:
	void initWindow(Engine& engine);

	void calcAspect();

	InputManager mInputManager;

	unsigned mWidth, mHeight;
	float mAspect;

	GLFWwindow* mGlfwWindow;
};

#endif
