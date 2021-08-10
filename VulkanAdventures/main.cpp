#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "vulkan-1")

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "src\ARWindow.hpp"
#include "src\VulkanInitializer\VulkanInstanceManager.h"
#include "src\VulkanInitializer\VulkanInitializer.hpp"
#include ".\src\Logger.h"

std::unique_ptr<va::VAWindow> window;
//std::unique_ptr<va::VulkanInitializer> vulkanSetup;
std::unique_ptr<va::VulkanInstanceManager> vInstance;

int main(int args1, char** args2) {
	try {
		va::Logger::Init();
		window = std::make_unique<va::VAWindow>(2048, 1024, "My Window");
		//vulkanSetup = std::make_unique<va::VulkanInitializer>(window->GetRawWindow(), "First Application", "Vulkan Engine");
		vInstance = std::make_unique<va::VulkanInstanceManager>("My Application", "My Engine");
	}
	catch (const std::exception& e) {
		std::cout << "Caught an Exception: " << e.what() << std::endl;
		LOGGER_CRITICAL("Caught an Exception: ", e.what());
		return EXIT_FAILURE;
	}


	while (!glfwWindowShouldClose(window->GetRawWindow()))
	{
		window->PollEvents();
	}
	
	return EXIT_SUCCESS;
};