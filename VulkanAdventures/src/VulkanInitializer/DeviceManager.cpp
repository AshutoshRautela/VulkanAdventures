#include "DeviceManager.h"

namespace va {

	DeviceManager::DeviceManager(const VkInstance& vkInstance, const std::shared_ptr<QueueManager>& queueManager) : 
		_vkInstance(vkInstance),
		_queueManager(queueManager),
		_vkPhysicalDevice { VK_NULL_HANDLE }
	{

	}

	std::vector<VkPhysicalDevice> DeviceManager::getAvailableDevices() {
		if (this->_availablePhysicalDevices.size() == 0) {
			uint32_t physicalDevicesCount = 0;
			vkEnumeratePhysicalDevices(this->_vkInstance, &physicalDevicesCount, nullptr);

			this->_availablePhysicalDevices = std::vector<VkPhysicalDevice>(physicalDevicesCount);
			vkEnumeratePhysicalDevices(this->_vkInstance, &physicalDevicesCount, this->_availablePhysicalDevices.data());

#if _DEBUG
			LOGGER_INFO("-----> Available Physical Devices <-----");
			for (const auto& apd : this->_availablePhysicalDevices) {
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(apd, &deviceProperties);

				LOGGER_INFO("{0} {1}", deviceProperties.deviceName, deviceProperties.deviceType);
			}
			LOGGER_INFO("----------");
#endif
		}
		return this->_availablePhysicalDevices;
	}

	void DeviceManager::pickPhysicalDevice() {
		this->getAvailableDevices();
		if (this->_availablePhysicalDevices.size() == 0) {
			throw std::runtime_error("No physical devices available");
		}

		// Check a Device is Suitable
		for(auto& apd : this->_availablePhysicalDevices) {
			if(this->isDeviceSuitable(apd) && this->_queueManager->checkAvailableQueueFamilies(apd)) {
				this->_vkPhysicalDevice = apd;
				break;
			}
		}

#if _DEBUG
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(this->_vkPhysicalDevice, &physicalDeviceProperties);

		LOGGER_INFO("Selected a Physical Device {0}", physicalDeviceProperties.deviceName);
#endif
	}

#if _DEBUG
	void DeviceManager::createLogicalDevice(std::vector<const char*> requiredExtensions, std::vector<const char*> validationLayers) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = this->_queueManager->getQueueFamilyIndices().graphisFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &this->_queueManager->getQueueFamilyIndices().queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo vkDeviceCreateInfo{};
		vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vkDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		vkDeviceCreateInfo.queueCreateInfoCount = 1;
		vkDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		vkDeviceCreateInfo.enabledExtensionCount = 0;

		vkDeviceCreateInfo.enabledLayerCount = validationLayers.size();
		vkDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

		VkResult res = vkCreateDevice(this->_vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &this->_vkDevice);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vk Logical Device");
		}
		LOGGER_INFO("Successfully created Logical Device");
	}
#endif

#if _RELEASE
	void DeviceManager::createLogicalDevice(std::vector<const char*> requiredExtensions) {

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = this->_queueManager->getQueueFamilyIndices().graphisFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &this->_queueManager->getQueueFamilyIndices().queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo vkDeviceCreateInfo{};
		vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vkDeviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		vkDeviceCreateInfo.queueCreateInfoCount = 1;
		vkDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		vkDeviceCreateInfo.enabledLayerCount = 0;

		VkResult res = vkCreateDevice(this->_vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &this->_vkDevice);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vk Logical Device");
		}
		LOGGER_INFO("Successfully created Logical Device");
	}
#endif

	bool DeviceManager::isDeviceSuitable(const VkPhysicalDevice& vkPhysicalDevice) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);
		
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
	}
	
	DeviceManager::~DeviceManager() {
		this->_availablePhysicalDevices.clear();
		if (this->_vkDevice != VK_NULL_HANDLE) {
			vkDestroyDevice(this->_vkDevice, nullptr);
		}
	}
}