#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <vector>
#include <stdexcept>
#include <optional>
#include <set>

#include "..\Logger.h"
#include ".\QueueManager.h"
#include ".\SwapchainManager.h"


namespace va {

	class DeviceManager
	{
	private:
		VkPhysicalDevice _vkPhysicalDevice;
		VkDevice _vkDevice;
		std::vector<VkPhysicalDevice> _availablePhysicalDevices;

		const VkInstance& _vkInstance;

		const std::shared_ptr<QueueManager>& _queueManager;
		const std::shared_ptr<SwapchainManager>& _swapchainManager;
	public:
		DeviceManager( const VkInstance& _vkInstance, const std::shared_ptr<QueueManager>& queueManager, const std::shared_ptr<SwapchainManager>& swapchainManager);
		~DeviceManager();

		//Deactivating the copy constructor and assignment operator
		DeviceManager(const DeviceManager&) = delete;
		const DeviceManager& operator=(const DeviceManager&) = delete;

		std::vector<VkPhysicalDevice> getAvailableDevices();
		
		void pickPhysicalDevice(const VkSurfaceKHR&);
#if _DEBUG
		void createLogicalDevice(std::vector<const char*> requiredExtensions, std::vector<const char*> validationLayers);
#endif
#if _RELEASE
		void createLogicalDevice(std::vector<const char*> requiredExtensions);
#endif
		bool isDeviceSuitable(const VkPhysicalDevice&);

		inline const VkPhysicalDevice& getPhysicalDevice() const { return this->_vkPhysicalDevice; }
		inline const VkDevice& getLogicalDevice() const { return this->_vkDevice; };
	};
}



