#include "engine.h"

Engine::Engine():
	mVulkanManager(mWindow)
{
	
}

Engine::~Engine() 
{

}

void onWindowResized(GLFWwindow* window, int width, int height) {
	if (width * height == 0) 
		return;

	Engine* eng = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	Window& engWindow = eng->getWindow();
	Camera& engCamera = eng->getCamera();

	engWindow.setDimens(width, height);
    engCamera.setAspect(engWindow.getAspect());
    engCamera.rebuildPerspective();
	eng->getVulkanManager().recreateSwapChain();
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	Engine* eng = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	Camera& engCamera = eng->getCamera();
    engCamera.updateOrientation(xpos, ypos);
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        std::cout<< "MBR ";
        switch(action)
        {
            case GLFW_RELEASE:
                  std::cout<< "RELEASE";

                break;
            case GLFW_PRESS:  
                std::cout<< "PRESS";

                break;
            case GLFW_REPEAT:
                  std::cout<< "REPEAT";
                break;
        }
        std::cout<<std::endl;
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        std::cout<<"MBL";
        switch(action)
        {
            case GLFW_RELEASE:
                  std::cout<< "RELEASE";
                break;
            case GLFW_PRESS:  
                std::cout<< "PRESS";
                break;
            case GLFW_REPEAT:
                  std::cout<< "REPEAT";
                break;
        }
        std::cout<<std::endl;
    }  
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  	Engine* eng = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
	Camera& engCamera = eng->getCamera(); 
    engCamera.updateFOV(yoffset);
    engCamera.rebuildPerspective();
    std::cout<< "SCROLL POS " << xoffset << " " << yoffset << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, VK_TRUE); 
		window = nullptr;
	} else if(key == GLFW_KEY_E && action == GLFW_PRESS) {
		std::cout<< "E PRESSED" <<std::endl;
	} else if(key == GLFW_KEY_E && action ==GLFW_REPEAT) {
		std::cout<< "E REPEAT" <<std::endl;
	}
}

void Engine::handleMovement(double dt) 
{
	InputManager& im = mWindow.getInputManager();
	if(im.keyPressed(GLFW_KEY_W))
        mCamera.moveStraight(1.0f, dt);
    if(im.keyPressed(GLFW_KEY_S))
        mCamera.moveStraight(-1.0f, dt);

    if(im.keyPressed(GLFW_KEY_D))
        mCamera.moveSideways(1.0f, dt);
    if(im.keyPressed(GLFW_KEY_A))
        mCamera.moveSideways(-1.0f, dt);
}

void Engine::init() 
{
	mWindow.initWindow(*this);
	mWindow.setWindowSizeCallback(onWindowResized);

	InputManager& inputManager = mWindow.getInputManager();
	inputManager.setCursorPosCallback(cursorPosCallback);
	inputManager.setScrollCallback(scrollCallback);
	inputManager.setMouseButtonCallback(mouseButtonCallback);
	inputManager.setKeyCallback(keyCallback);
	mVulkanManager.createVkInstance();

#ifdef AMVK_DEBUG
	mVulkanManager.enableDebug();
#endif

	mVulkanManager.createVkSurface(*mWindow.mGlfwWindow);
	mVulkanManager.createPhysicalDevice();
	mVulkanManager.createLogicalDevice();
	mVulkanManager.createSwapChain(mWindow);
	mVulkanManager.createImageViews();
	mVulkanManager.createRenderPass();
	mVulkanManager.createDescriptorSetLayout();
	mVulkanManager.createPipeline();
	mVulkanManager.createCommandPool();

	mVulkanManager.createDepthResources();
	mVulkanManager.createFramebuffers();

	mVulkanManager.createTextureImage();
	mVulkanManager.createTextureImageView();
	mVulkanManager.createTextureSampler();
	mVulkanManager.createVertexBuffer();
	mVulkanManager.createIndexBuffer();
	mVulkanManager.createUniformBuffer();
	mVulkanManager.createDescriptorPool();
	mVulkanManager.createDescriptorSet();


	mVulkanManager.createCommandBuffers();
	mVulkanManager.createSemaphores();
	LOG("INIT SUCCESSFUL");
}

Window& Engine::getWindow()
{
	return mWindow;
} 

TaskManager& Engine::getTaskManager() 
{
	return mTaskManager;
} 

VulkanManager& Engine::getVulkanManager()
{
	return mVulkanManager;
}

Timer& Engine::getTimer()
{
	return mTimer;
}

Camera& Engine::getCamera()
{
	return mCamera;
}
