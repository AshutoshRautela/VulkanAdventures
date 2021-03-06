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
#include "src\Renderer.h"
#include "src\Mesh.h"
#include "src\Renderer.h"

std::unique_ptr<va::VAWindow> window;
std::unique_ptr<va::VulkanManager> vManager;

std::unique_ptr<va::GraphicsPipeline> vGraphicsPipeline;
std::unique_ptr<va::Renderer> renderer;
std::unique_ptr<va::Drawing> vDrawing;


//va::GraphicsPipeline* vGraphicsPipeline;
va::Drawing* drawing;
std::unique_ptr<va::Mesh> mesh;

std::vector<va::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};
std::vector<uint16_t> indices = { 0 , 1 , 2 , 2 , 3 , 0 };

int main(int args1, char** args2) {
	try {
		va::Logger::Init();
		window = std::make_unique<va::VAWindow>(2048, 1024, "My Window");
		vManager = std::make_unique<va::VulkanManager>(window, "My Application", "My Engine");
		vGraphicsPipeline = std::make_unique<va::GraphicsPipeline>(vManager->getVkDevice());
		renderer = std::make_unique<va::Renderer>(vManager->getPhysicalDevice(), vManager->getVkDevice(), std::move(va::Mesh(std::move(vertices), std::move(indices))));
		vGraphicsPipeline->startGraphicsPipelineProcess(
			"./src/Shaders/vert.spv",
			"./src/Shaders/frag.spv",
			vManager->getSwapchainSurfaceFormat(),
			vManager->getSwapchainExtent()
		);
		vDrawing = std::make_unique<va::Drawing>(renderer, vGraphicsPipeline, vManager, window);
		vDrawing->prapareFrameBuffersAndCommandPool();

		
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