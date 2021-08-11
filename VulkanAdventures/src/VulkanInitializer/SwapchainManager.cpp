#include "SwapchainManager.h"

namespace va {

	bool SwapchainManager::checkSwapchainSupport(const VkPhysicalDevice& vkPhysicalDevice) {

		uint32_t availableExtensionCount = 0;
		std::vector<VkExtensionProperties> availableExtensions;

		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &availableExtensionCount, nullptr);

		availableExtensions.resize(availableExtensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &availableExtensionCount, availableExtensions.data());

		for (const auto& rex : this->_swapchainRequiredExtensions) {
			bool found = false;
			for (const auto& aex : availableExtensions) {
				if (strcmp(rex, aex.extensionName) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
#if _DEBUG
				VkPhysicalDeviceProperties deviceProps;
				vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProps);
				LOGGER_ERROR("Swapchain Extension not available for device: {0} {1}", deviceProps.deviceName, rex);
#endif
				return false;
			}
		}
#if _DEBUG
		LOGGER_INFO("Swapchain Extension is supported!");
#endif
		return true;
	}

}