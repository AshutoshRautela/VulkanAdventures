#include "Drawing.h"

namespace va {

	Drawing::Drawing(const std::unique_ptr<GraphicsPipeline>& graphicsPipeline, const std::unique_ptr<VulkanManager>& vulkanManager) : _vkDevice(vulkanManager->getVkDevice()), _graphicsPipeline(graphicsPipeline), _vulkanManager(vulkanManager) {}

	void Drawing::createFrameBuffers() {
		this->_swapchainframebuffers.resize(this->_vulkanManager->getSwapchainImageViews().size());

		size_t i = 0;
		for (const VkImageView& iv : this->_vulkanManager->getSwapchainImageViews()) {
			VkImageView atttachments[] = { iv };

			VkFramebufferCreateInfo vkFramebufferCreateInfo{};
			vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vkFramebufferCreateInfo.renderPass = this->_graphicsPipeline->getRenderPass();
			vkFramebufferCreateInfo.attachmentCount = 1;
			vkFramebufferCreateInfo.pAttachments = atttachments;
			vkFramebufferCreateInfo.width = this->_vulkanManager->getSwapchainExtent().width;
			vkFramebufferCreateInfo.height = this->_vulkanManager->getSwapchainExtent().height;
			vkFramebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(this->_vkDevice, &vkFramebufferCreateInfo, nullptr, &this->_swapchainframebuffers[i++]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Framebuffer");
			}
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Frame Buffers");
#endif // _DEBUG
	}

	void Drawing::createCommandPool() {
		VkCommandPoolCreateInfo vkCommandPoolCreateInfo{};
		vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vkCommandPoolCreateInfo.queueFamilyIndex = this->_vulkanManager->getQueueFamilyIndices().graphisFamily.value();
		//vkCommandPoolCreateInfo.flags = 0;

		if (vkCreateCommandPool(this->_vkDevice, &vkCommandPoolCreateInfo, nullptr, &this->_vkCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("Error creating commnd pool");
		}

#if _DEBUG || _RELEASE
		LOGGER_INFO("Successfully created command pool for graphics queue");
#endif // _DEBUG

	}

	void Drawing::prepareCommandBuffers() {
		this->createCommandBuffers();
		this->startCommandBufferRecording();
		this->beginRenderPass();
		this->draw();
		this->endRenderPass();
		this->stopCommandBufferRecording();
	}

	void Drawing::createCommandBuffers() {
		this->_vkCommandBuffers.resize(this->_swapchainframebuffers.size());

		VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
		vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkCommandBufferAllocateInfo.commandPool = this->_vkCommandPool;
		vkCommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(this->_vkCommandBuffers.size());
		vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(this->_vkDevice, &vkCommandBufferAllocateInfo, this->_vkCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Command Buffers");
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Command Buffers");
#endif // _DEBUG

	}

	void Drawing::startCommandBufferRecording() {
		for (const VkCommandBuffer& cb : this->_vkCommandBuffers) {
			VkCommandBufferBeginInfo vkCommandBufferBeginInfo{};
			vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//vkCommandBufferBeginInfo.flags = 0;
			//vkCommandBufferBeginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(cb, &vkCommandBufferBeginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}

#if _DEBUG
		LOGGER_INFO("Successfully started Command Buffer Recording");
#endif // _DEBUG

	}

	void Drawing::beginRenderPass() {
		int i = 0;
		for (const VkCommandBuffer& cb : this->_vkCommandBuffers) {
			VkRenderPassBeginInfo vkRenderPassBeginInfo{};
			vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vkRenderPassBeginInfo.renderPass = this->_graphicsPipeline->getRenderPass();
			vkRenderPassBeginInfo.framebuffer = this->_swapchainframebuffers[i];
			vkRenderPassBeginInfo.renderArea.offset = { 0 , 0 };
			vkRenderPassBeginInfo.renderArea.extent = this->_vulkanManager->getSwapchainExtent();

			VkClearValue vkClearValue = { {{0.0f, 0.0f, 0.0f, 0.0f }} };
			vkRenderPassBeginInfo.clearValueCount = 1;
			vkRenderPassBeginInfo.pClearValues = &vkClearValue;

			vkCmdBeginRenderPass(cb, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			i++;
		}
	}

	void Drawing::draw() {
		for (const VkCommandBuffer& cb : this->_vkCommandBuffers) {
			vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, this->_graphicsPipeline->getGraphicsPipeline());
			vkCmdDraw(cb, 3, 1, 0, 0);
		}

#if _DEBUG
		LOGGER_INFO("Drawing");
#endif // _DEBUG
	}

	void Drawing::endRenderPass() {
		for (const VkCommandBuffer& cb : this->_vkCommandBuffers) {
			vkCmdEndRenderPass(cb);
		}
#if _DEBUG
		LOGGER_INFO("Ended Render Pass");
#endif // _DEBUG

	}

	void Drawing::stopCommandBufferRecording() {
		for (const VkCommandBuffer& cb : this->_vkCommandBuffers) {
			if (vkEndCommandBuffer(cb) != VK_SUCCESS) {
				throw std::runtime_error("Failed to end Command Buffer");
			}
		}
#if _DEBUG
		LOGGER_INFO("Stopped Commend Buffer Recording");
#endif // _DEBUG
	}

	void Drawing::createSyncObjects() {
		this->_imageAvailableSemaphores.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->_renderFinishedSemaphores.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->_inFlightFences.resize(this->MAX_FRAMES_IN_FLIGHT);
		this->_imagesInFlight.resize(this->_vulkanManager->getSwapchainImages().size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo vkSemaphoreCreateInfo{};
		vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo vkFenceCreateInfo{};
		vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(this->_vkDevice, &vkSemaphoreCreateInfo, nullptr, &this->_imageAvailableSemaphores[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create semaphore");
			}

			if (vkCreateSemaphore(this->_vkDevice, &vkSemaphoreCreateInfo, nullptr, &this->_renderFinishedSemaphores[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create semaphore");
			}
			
			if (vkCreateFence(this->_vkDevice, &vkFenceCreateInfo, nullptr, &this->_inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create fence");
			}
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Semaphores and Fences");
#endif // _DEBUG

	}

	void Drawing::drawFrame() {
		vkWaitForFences(this->_vkDevice, 1, &this->_inFlightFences[this->_currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(this->_vkDevice, 1, &this->_inFlightFences[this->_currentFrame]);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(this->_vkDevice, this->_vulkanManager->getSwapChain(), INT32_MAX, this->_imageAvailableSemaphores[this->_currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (this->_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(this->_vkDevice, 1, &this->_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}

		VkSubmitInfo vkSubmitInfo{};
		vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { this->_imageAvailableSemaphores[this->_currentFrame]};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		vkSubmitInfo.waitSemaphoreCount = 1;
		vkSubmitInfo.pWaitSemaphores = waitSemaphores;
		vkSubmitInfo.pWaitDstStageMask = waitStages;
		vkSubmitInfo.commandBufferCount = 1;
		vkSubmitInfo.pCommandBuffers = &this->_vkCommandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { this->_renderFinishedSemaphores[this->_currentFrame]};
		vkSubmitInfo.signalSemaphoreCount = 1;
		vkSubmitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(this->_vulkanManager->getGraphicsQueue(), 1, &vkSubmitInfo, this->_inFlightFences[this->_currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit Queue");
		}

		VkPresentInfoKHR vkPresentInfoKHR{};
		vkPresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		vkPresentInfoKHR.waitSemaphoreCount = 1;
		vkPresentInfoKHR.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { this->_vulkanManager->getSwapChain() };
		vkPresentInfoKHR.swapchainCount = 1;
		vkPresentInfoKHR.pSwapchains = swapchains;
		vkPresentInfoKHR.pImageIndices = &imageIndex;
		vkPresentInfoKHR.pResults = nullptr;

		vkQueuePresentKHR(this->_vulkanManager->getPresentQueue(), &vkPresentInfoKHR);
		vkQueueWaitIdle(this->_vulkanManager->getPresentQueue());

		this->_currentFrame = (this->_currentFrame + 1) % this->MAX_FRAMES_IN_FLIGHT;
	}

	Drawing::~Drawing() {
		for (size_t i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(this->_vkDevice, this->_imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(this->_vkDevice, this->_renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(this->_vkDevice, this->_inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(this->_vkDevice, this->_vkCommandPool, nullptr);
		for (VkFramebuffer fb : this->_swapchainframebuffers) {
			vkDestroyFramebuffer(this->_vkDevice, fb, nullptr);
		}
	}
}