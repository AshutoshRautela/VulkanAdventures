#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <vector>
#include <stdexcept>
#include <optional>

#include "..\Logger.h"
#include ".\QueueManager.h"

namespace va {

	class DeviceManager
	{
	private:
		VkPhysicalDevice _vkPhysicalDevice;
		VkDevice _vkDevice;
		std::vector<VkPhysicalDevice> _availablePhysicalDevices;

		const VkInstance& _vkInstance;
		const std::shared_ptr<QueueManager>& _queueManager;
	public:
		DeviceManager( const VkInstance& _vkInstance, const std::shared_ptr<QueueManager>& queueManager);
		~DeviceManager();

		//Deactivating the copy constructor and assignment operator
		DeviceManager(const DeviceManager&) = delete;
		const DeviceManager& operator=(const DeviceManager&) = delete;

		std::vector<VkPhysicalDevice> getAvailableDevices();
		
		void pickPhysicalDevice();
#if _DEBUG
		void createLogicalDevice(std::vector<const char*> requiredExtensions, std::vector<const char*> validationLayers);
#endif
#if _RELEASE
		void createLogicalDevice(std::vector<const char*> requiredExtensions);
#endif
		bool isDeviceSuitable(const VkPhysicalDevice&);
		inline const VkDevice& getLogicalDevice() const { return this->_vkDevice; };
	};
}


