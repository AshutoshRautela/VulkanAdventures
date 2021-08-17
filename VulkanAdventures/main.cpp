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
#include <fstream>

#include "src\ARWindow.hpp"
#include "src\VulkanInitializer\VulkanManager.h"
#include ".\src\Logger.h"

#include "src\GraphicsPipeline.h"
#include "src\Drawing.h"

std::shared_ptr<va::VAWindow> window;
std::unique_ptr<va::VulkanManager> vManager;

std::unique_ptr<va::GraphicsPipeline> vGraphicsPipeline;
std::unique_ptr<va::Drawing> vDrawing;

//va::GraphicsPipeline* vGraphicsPipeline;
va::Drawing* drawing;

int main(int args1, char** args2) {
	try {
		va::Logger::Init();
		window = std::make_shared<va::VAWindow>(2048, 1024, "My Window");
		vManager = std::make_unique<va::VulkanManager>(window, "My Application", "My Engine");
		vGraphicsPipeline = std::make_unique<va::GraphicsPipeline>(vManager->getVkDevice());
		vGraphicsPipeline->startGraphicsPipelineProcess(
			"./src/Shaders/vert.spv",
			"./src/Shaders/frag.spv",
			vManager->getSwapchainSurfaceFormat(),
			vManager->getSwapchainExtent()
		);
		vDrawing = std::make_unique<va::Drawing>(vGraphicsPipeline, vManager);
		vDrawing->createFrameBuffers();
		vDrawing->createCommandPool();
		vDrawing->prepareCommandBuffers();
		vDrawing->createSyncObjects();

		while (!glfwWindowShouldClose(window->GetRawWindow()))
		{
			window->PollEvents();
			vDrawing->drawFrame();
		}

		vkDeviceWaitIdle(vManager->getVkDevice());
	}
	catch (const std::exception& e) {
		LOGGER_CRITICAL("Caught an Exception: {0}", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
};