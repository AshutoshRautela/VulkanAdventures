#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include "./VulkanInstanceManager.h"
#include "./ValidationManager.h"
#include "./ExtensionManager.h"
#include "./DeviceManager.h"
#include "./QueueManager.h"
#include "./SwapchainManager.h"

#include "../ARWindow.hpp";

namespace va {
	class VulkanManager
	{
	public:
		VulkanManager(const std::unique_ptr<VAWindow>& ,std::string, std::string);

		VulkanManager(const VulkanManager&) = delete;
		const VulkanManager& operator=(const VulkanManager&) = delete;

		// Recreate Swap Chain
		void clearSwapChainAndImageViews();
		void createSwapChainAndImageViews();

		inline const VkDevice& getVkDevice() const { return this->_deviceManager->getLogicalDevice(); }
		inline const VkExtent2D& getSwapchainExtent() const { return this->_swapchainManager->getExtent(); }
		inline const VkSurfaceFormatKHR getSwapchainSurfaceFormat() const { return this->_swapchainManager->getSwapchainSurfaceFormat(); }
		inline const QueueFamilyIndices& getQueueFamilyIndices() const { return this->_queueManager->getQueueFamilyIndices(); }
		inline const std::vector<VkImage>& getSwapchainImages() const { return this->_swapchainManager->getSwapchainImages(); }
		inline const std::vector<VkImageView>& getSwapchainImageViews() const { return this->_swapchainManager->getSwapchainImageViews(); }
		inline const VkSwapchainKHR& getSwapChain() const { return this->_swapchainManager->getSwapchain(); }
		inline const VkQueue& getGraphicsQueue() const { return this->_queueManager->getGraphicsQueue(); }
		inline const VkQueue& getPresentQueue() const { return this->_queueManager->getPresentQueue(); }

	private:
		std::unique_ptr<VulkanInstanceManager> _instanceManager;
		const std::unique_ptr<VAWindow>& _vaWindow;

		std::unique_ptr<ExtensionManager> _extensionManager;
		std::unique_ptr<ValidationManager> _validationManager;
		std::unique_ptr<DeviceManager> _deviceManager;
		std::shared_ptr<QueueManager> _queueManager;
		std::shared_ptr<SwapchainManager> _swapchainManager;
	};
}



