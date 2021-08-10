#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <string>
#include <stdexcept>

#include "../Logger.h"

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
		VulkanInstanceManager(
			std::string applicationName,
			std::string engineName,
			std::vector<const char*> requiredExtensions
		);
		VulkanInstanceManager(
			std::string applicationName,
			std::string engineName,
			std::vector<const char*> requiredExtensions,
			std::vector<const char*> validationLayers,
			VkDebugUtilsMessengerCreateInfoEXT* messengerInfo
		);
		~VulkanInstanceManager();

		// Deactivating the Copy Constructor and the Assignment Operator 
		VulkanInstanceManager(const VulkanInstanceManager&) = delete;
		const VulkanInstanceManager& operator=(const VulkanInstanceManager&) = delete;

		inline VkInstance getVkInstance() { return this->_vkInstance; }
	};

}


