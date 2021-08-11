#include "VulkanInstanceManager.h"

namespace va {

	VulkanInstanceManager::VulkanInstanceManager(
		std::string applicationName,
		std::string engineName,
		std::vector<const char*> requiredExtensions
	): VulkanInstanceManager(
		applicationName,
		engineName,
		requiredExtensions,
		std::vector<const char*>(),
		{}
	) {}

	VulkanInstanceManager::VulkanInstanceManager(
		std::string applicationName,
		std::string engineName,
		std::vector<const char*> requiredExtensions,
		std::vector<const char*> validationLayers,
		VkDebugUtilsMessengerCreateInfoEXT* messengerInfo
	) : _applicationName{ applicationName }, _engineName{ engineName } {

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

		this->_vkInstanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
		this->_vkInstanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

#if _RELEASE
		this->_vkInstanceCreateInfo.enabledLayerCount = 0;
		this->_vkInstanceCreateInfo.pNext = nullptr;
		LOGGER_INFO("Skipped Validation Layers");
#endif // _RELEASE

#if _DEBUG
		LOGGER_INFO("Initializing Validation Layers for Debug Mode!!")
		this->_vkInstanceCreateInfo.enabledLayerCount = validationLayers.size();
		this->_vkInstanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		this->_vkInstanceCreateInfo.pNext = messengerInfo;
#endif

		VkResult res = vkCreateInstance(&this->_vkInstanceCreateInfo, nullptr, &this->_vkInstance);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan Instance");
		}

#if _DEBUG
		LOGGER_INFO("Successfully created a Vulkan Instance");
#endif
	}

	void VulkanInstanceManager::createSurface(GLFWwindow* window) {
		VkResult res = glfwCreateWindowSurface(this->_vkInstance, window, nullptr, &this->_vkSurface);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Window Surface");
		}
#if _DEBUG
		LOGGER_INFO("Successfully created Window Surface");
#endif // _DEBUG

	}

	VulkanInstanceManager::~VulkanInstanceManager() {
		vkDestroySurfaceKHR(this->_vkInstance, this->_vkSurface, nullptr);
		vkDestroyInstance(this->_vkInstance, nullptr);
#if _DEBUG
		LOGGER_INFO("Vulkan Instance destroyed");
#endif
	}
}