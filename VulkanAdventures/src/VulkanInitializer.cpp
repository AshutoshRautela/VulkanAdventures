#include "VulkanInitializer.hpp"

namespace va {
	VulkanInitializer::VulkanInitializer(std::string appName, std::string engineName) throw (std::runtime_error) : applicationName{ appName }, engineName{engineName} {
		this->prepareInstance();
	}

	VulkanInitializer* VulkanInitializer::prepareInstance() throw (std::runtime_error) {
		std::vector<VkExtensionProperties> supportedExtensions = this->getSupportVkExtensions();
		std::pair<uint32_t, const char**> glfwExtensions = this->getGlfwExtensions();

		for (int i = 0; i < glfwExtensions.first; ++i) {
			bool foundSupportedExtension = false;
			for (VkExtensionProperties& vkExtension: supportedExtensions) {
				if (strcmp(glfwExtensions.second[i], vkExtension.extensionName) == 0) {
					foundSupportedExtension = true;
					break;
				}
			}
			if (!foundSupportedExtension) {
				throw std::runtime_error("Couldn't fnd supported extension for GFLW: " + std::string(glfwExtensions.second[i]));
			}
		}
		std::cout << "Glfw extensions are supported" << std::endl;

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = this->applicationName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = this->engineName.c_str();
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0); // To be tested

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensions.first;
		createInfo.ppEnabledExtensionNames = glfwExtensions.second;
		createInfo.enabledLayerCount = 0;

		if (vkCreateInstance(&createInfo, nullptr, &this->_vkInstance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan Instance");
		}
		std::cout << "Created Vulkan Instance Successfully" << std::endl;
		return this;
	}

	VulkanInitializer::~VulkanInitializer() {
		if (this->_vkInstance) {
			vkDestroyInstance(this->_vkInstance, nullptr);
		}
	}
}