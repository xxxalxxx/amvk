#ifndef AMVK_INPUT_MANAGER_H
#define AMVK_INPUT_MANAGER_H

#include <GLFW/glfw3.h> 
#include <exception>
class InputManager {
public:
	InputManager();
	InputManager(GLFWwindow& window);
	virtual ~InputManager();
	void init();
	inline void pollEvents() { glfwPollEvents(); }
	void setGlfwWindow(GLFWwindow& window);
	void assertGlfwWindowIsValid();
	void setFramebufferSizeCallback(GLFWframebuffersizefun cbfun);
	void setScrollCallback(GLFWscrollfun cbfun);
	void setKeyCallback(GLFWkeyfun cbfun);
	void setCursorPosCallback(GLFWcursorposfun cbfun);
	void setMouseButtonCallback(GLFWmousebuttonfun cbfun);

private:
	GLFWwindow* mGlfwWindow;
};



#endif


