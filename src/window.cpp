#include "window.h"

Window::Window():
	mWidth(DEFAULT_WINDOW_WIDTH), 
	mHeight(DEFAULT_WINDOW_HEIGHT),
	mGlfwWindow(nullptr)
{
	calcAspect();
}

Window::Window(unsigned w, unsigned h): 
	mWidth(w), 
	mHeight(h),
	mGlfwWindow(nullptr)
{
	calcAspect();
}

Window::~Window() 
{
	if (mGlfwWindow)
		glfwSetWindowShouldClose(mGlfwWindow, GLFW_TRUE); 
	glfwTerminate();
}

void Window::initWindow(Engine& engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	mGlfwWindow = glfwCreateWindow(mWidth, mHeight, "Learning Vulkan", nullptr, nullptr);

	if (!mGlfwWindow) {
		glfwTerminate();
		throw std::runtime_error("GLFW window initialization failed");
	}

	//glfwSetInputMode(mGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(mGlfwWindow, GLFW_STICKY_KEYS, 1);
	mInputManager.setGlfwWindow(*mGlfwWindow);
	mInputManager.init();
	glfwSetWindowUserPointer(mGlfwWindow, (void*) &engine);
}


void Window::calcAspect() 
{
	mAspect = (float) mWidth / mHeight;
}

InputManager& Window::getInputManager()
{
	return mInputManager;
}

unsigned Window::getWidth() const  
{
	return mWidth;
}

unsigned Window::getHeight() const 
{
	return mHeight;
}

float Window::getAspect() const
{
	return mAspect;
}

void Window::setWidth(unsigned w) 
{
	mWidth = w;
	calcAspect();
}

void Window::setHeight(unsigned h) 
{
	mHeight = h;
	calcAspect();
}

void Window::setDimens(unsigned w, unsigned h) 
{
	mWidth = w;
	mHeight = h;
	calcAspect();
}
