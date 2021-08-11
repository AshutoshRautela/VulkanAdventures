#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <vector>
#include <algorithm>
#include <iostream>

#include "../Logger.h"

namespace va {
	class ExtensionManager
	{
	private:
		std::vector<const char*> _requiredExtensions;
		std::vector<VkExtensionProperties> _availableExtensions;

	public:
		ExtensionManager() {};
		~ExtensionManager();
		ExtensionManager(const ExtensionManager&) = delete;
		const ExtensionManager& operator=(const ExtensionManager&) = delete;

		std::vector<const char*> getGlfwExtensions();
		std::vector<const char*> getInstanceRequiredExtensions();
		std::vector<VkExtensionProperties> getInstanceAvailableExtensions();
		bool isRequiredExtensionsSupported();
	};
}



