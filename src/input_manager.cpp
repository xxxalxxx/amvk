#include "input_manager.h"
#include <iostream>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE); 
		window = nullptr;
	} else if(key == GLFW_KEY_E && action == GLFW_PRESS) {
		std::cout<< "E PRESSED" <<std::endl;
	} else if(key == GLFW_KEY_E && action ==GLFW_REPEAT) {
		std::cout<< "E REPEAT" <<std::endl;
	}
}


InputManager::InputManager():
	mGlfwWindow(nullptr)
{

}

InputManager::InputManager(GLFWwindow& glfwWindow): 
	mGlfwWindow(&glfwWindow)
{

}

InputManager::~InputManager()
{

}

void InputManager::setGlfwWindow(GLFWwindow& glfwWindow)
{
	mGlfwWindow = &glfwWindow;
}

void InputManager::assertGlfwWindowIsValid() 
{
	if (!mGlfwWindow)
		throw std::runtime_error("GLFWWindow cannot be NULL");
}

void InputManager::setFramebufferSizeCallback(GLFWframebuffersizefun cbfun)
{
	assertGlfwWindowIsValid();
	glfwSetFramebufferSizeCallback(mGlfwWindow, cbfun);
}

void InputManager::setScrollCallback(GLFWscrollfun cbfun)
{   
	assertGlfwWindowIsValid();
	glfwSetScrollCallback(mGlfwWindow, cbfun);
}

void InputManager::setKeyCallback(GLFWkeyfun cbfun)
{
	assertGlfwWindowIsValid();
	glfwSetKeyCallback(mGlfwWindow, cbfun); 
}

void InputManager::setCursorPosCallback(GLFWcursorposfun cbfun)
{
	assertGlfwWindowIsValid();
	glfwSetCursorPosCallback(mGlfwWindow, cbfun);
}

void InputManager::setMouseButtonCallback(GLFWmousebuttonfun cbfun)
{
	assertGlfwWindowIsValid();
	glfwSetMouseButtonCallback(mGlfwWindow, cbfun);
}

void InputManager::init() 
{
	setKeyCallback(keyCallback);
}
