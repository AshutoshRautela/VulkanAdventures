#pragma once

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
		VulkanManager(const std::shared_ptr<VAWindow>& ,std::string, std::string);

		VulkanManager(const VulkanManager&) = delete;
		const VulkanManager& operator=(const VulkanManager&) = delete;

	private:
		std::unique_ptr<VulkanInstanceManager> _instanceManager;

		std::unique_ptr<ExtensionManager> _extensionManager;
		std::unique_ptr<ValidationManager> _validationManager;
		std::unique_ptr<DeviceManager> _deviceManager;
		std::shared_ptr<QueueManager> _queueManager;
		std::shared_ptr<SwapchainManager> _swapchainManager;
	};
}



