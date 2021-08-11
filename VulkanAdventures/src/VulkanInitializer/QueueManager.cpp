#include "QueueManager.h"

namespace va {

	bool QueueManager::checkAvailableQueueFamilies(const VkPhysicalDevice& vkPhysicalDevice, const VkSurfaceKHR& vkSurface) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& qf : queueFamilies) {
			if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				this->_queueFamilyIndices.graphisFamily = i;
			}

			VkBool32 presentSupport;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentSupport);
			if (presentSupport) {
				this->_queueFamilyIndices.presentFamily = i;
			}

			i++;
		}
		return this->_queueFamilyIndices.isAvailable();
	}

	void QueueManager::UpdateQueues(const VkDevice& _vkDevice) {
		vkGetDeviceQueue(_vkDevice, this->_queueFamilyIndices.graphisFamily.value(), 0, &this->_graphicsQueue);
		vkGetDeviceQueue(_vkDevice, this->_queueFamilyIndices.presentFamily.value(), 0, &this->_presentQueue);
		LOGGER_INFO("Initialized Queues");
	}
}