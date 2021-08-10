#include "VulkanInstanceManager.h"

namespace va {

	VulkanInstanceManager::VulkanInstanceManager(std::string appName, std::string engineName) : _applicationName{ appName }, _engineName{ engineName } {

		// Creating the Application Information
		this->_vkApplicationInfo = { };

		this->_vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		this->_vkApplicationInfo.pApplicationName = this->_applicationName.c_str();
		this->_vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		this->_vkApplicationInfo.pEngineName = this->_engineName.c_str();
		this->_vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		this->_vkApplicationInfo.apiVersion = VK_API_VERSION_1_0;

		// Creating the Instance Information
		this->_vkInstanceCreateInfo = {};

		this->_vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		this->_vkInstanceCreateInfo.pApplicationInfo = &this->_vkApplicationInfo;

		std::pair<uint32_t, const char**> glfwExtenions = va::ExtensionManager::getGLFWExtensions();
		this->_vkInstanceCreateInfo.enabledExtensionCount = glfwExtenions.first;
		this->_vkInstanceCreateInfo.ppEnabledExtensionNames = glfwExtenions.second;

		this->_vkInstanceCreateInfo.enabledLayerCount = 0;

		VkResult res = vkCreateInstance(&this->_vkInstanceCreateInfo, nullptr, &this->_vkInstance);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan Instance");
		}

		LOGGER_INFO("Successfully created a Vulkan Instance");
	}

	VulkanInstanceManager::~VulkanInstanceManager() {
		vkDestroyInstance(this->_vkInstance, nullptr);
		LOGGER_INFO("Vulkan Instance destroyed");
	}
}