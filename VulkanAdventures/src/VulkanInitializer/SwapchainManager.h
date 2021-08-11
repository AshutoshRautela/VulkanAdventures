#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <vector>

#include "..\Logger.h"

namespace va {
	class SwapchainManager
	{
	private:
		std::vector<const char*> _swapchainRequiredExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	public:
		bool checkSwapchainSupport(const VkPhysicalDevice&);
		inline const std::vector<const char*>& requiredDeviceExtensions() { return this->_swapchainRequiredExtensions; }
	};
}



