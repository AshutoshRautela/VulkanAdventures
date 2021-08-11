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
#include "src\VulkanInitializer\VulkanManager.h"
#include ".\src\Logger.h"

std::shared_ptr<va::VAWindow> window;
std::unique_ptr<va::VulkanManager> vManager;

int main(int args1, char** args2) {
	try {
		va::Logger::Init();
		window = std::make_shared<va::VAWindow>(2048, 1024, "My Window");
		vManager = std::make_unique<va::VulkanManager>(window, "My Application", "My Engine");
	}
	catch (const std::exception& e) {
		LOGGER_CRITICAL("Caught an Exception: {0}", e.what());
		return EXIT_FAILURE;
	}

	while (!glfwWindowShouldClose(window->GetRawWindow()))
	{
		window->PollEvents();
	}
	
	return EXIT_SUCCESS;
};