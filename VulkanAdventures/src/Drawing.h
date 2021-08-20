#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include<vector>
#include<memory>

#include "Logger.h"
#include "GraphicsPipeline.h"
#include "VulkanInitializer\VulkanManager.h"

namespace va {

	class Drawing
	{
	private:
		std::vector<VkFramebuffer> _swapchainframebuffers;
		const std::unique_ptr<GraphicsPipeline>& _graphicsPipeline;
		const std::unique_ptr<VulkanManager>& _vulkanManager;
		const std::unique_ptr<VAWindow>& _vaWindow;

		const VkDevice& _vkDevice;

		VkCommandPool _vkCommandPool;
		std::vector<VkCommandBuffer> _vkCommandBuffers;

		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
		std::vector<VkFence> _imagesInFlight;

		void createCommandBuffers();
		void startCommandBufferRecording();
		void beginRenderPass();
		void draw();
		void endRenderPass();
		void stopCommandBufferRecording();

		void createFrameBuffers();
		void createCommandPool();
		void prepareCommandBuffers();
		void createSyncObjects();

		const int MAX_FRAMES_IN_FLIGHT = 2;
		size_t _currentFrame = 0;

	public:
		Drawing(const std::unique_ptr<GraphicsPipeline>&, const std::unique_ptr<VulkanManager>&, const std::unique_ptr<VAWindow>&);
		~Drawing();

		void prapareFrameBuffersAndCommandPool();
		bool _frameBufferResized = false;

		void drawFrame();


		void clearFrameBuffers();
		void clearCommandBuffers();

	};
}



