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
#include "src\VulkanInitializer.hpp"

std::unique_ptr<va::VAWindow> window;
std::unique_ptr<va::VulkanInitializer> vulkanSetup;

int main(int args1, char** args2) {
	try {
		window = std::make_unique<va::VAWindow>(2048, 1024, "My Window");
		vulkanSetup = std::make_unique<va::VulkanInitializer>(window->GetRawWindow(), "First Application", "Vulkan Engine");
	}
	catch (const std::exception& e) {
		std::cout << "Caught an Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	while (!glfwWindowShouldClose(window->GetRawWindow()))
	{
		window->PollEvents();
	}
	
	return EXIT_SUCCESS;
};