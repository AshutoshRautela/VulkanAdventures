#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <string>
#include <stdexcept>

#include "../Logger.h"
#include "ExtensionManager.h"

namespace va {

	class VulkanInstanceManager
	{
	private:
		VkApplicationInfo _vkApplicationInfo;
		VkInstanceCreateInfo _vkInstanceCreateInfo;
		VkInstance _vkInstance;

		std::string _applicationName;
		std::string _engineName;

	public:
		VulkanInstanceManager(std::string, std::string);
		~VulkanInstanceManager();

		// Deactivating the Copy Constructor and the Assignment Operator 
		VulkanInstanceManager(const VulkanInstanceManager&) = delete;
		const VulkanInstanceManager& operator=(const VulkanInstanceManager&) = delete;
	};

}


