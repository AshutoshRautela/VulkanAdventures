#include "VulkanInitializer.hpp"

namespace va {
	VulkanInitializer::VulkanInitializer(std::string appName, std::string engineName): applicationName{ appName }, engineName{engineName} {
		this->prepareInstance();
	}

	std::vector<const char*> VulkanInitializer::requestValidationLayers(){
		std::vector<VkLayerProperties> validationLayers = this->getValidationLayers();
		const std::vector<const char*> requiredvalidationLayers = { "VK_LAYER_KHRONOS_validation" };

		for (const char* requiredLayer : requiredvalidationLayers) {
			bool foundLayer = false;
			for (const VkLayerProperties& validationLayer : validationLayers) {
				if (strcmp(validationLayer.layerName, requiredLayer) == 0) {
					foundLayer = true;
					break;
				}
			}
			if (!foundLayer) {
				throw std::runtime_error("Requested Validation Layers are not available");
			}
		}
		return requiredvalidationLayers;
	}

	std::vector<const char*> VulkanInitializer::getCompitableGlfwExtensions() {
		std::vector<VkExtensionProperties> supportedExtensions = this->getSupportVkExtensions();
		std::vector<const char*> glfwExtensions = this->getGlfwExtensions();

		for (const char* glfwExtension: glfwExtensions) {
			bool foundSupportedExtension = false;
			for (VkExtensionProperties& vkExtension : supportedExtensions) {
				if (strcmp(glfwExtension, vkExtension.extensionName) == 0) {
					foundSupportedExtension = true;
					break;
				}
			}
			if (!foundSupportedExtension) {
				throw std::runtime_error("Couldn't fnd supported extension for GFLW: " + std::string(glfwExtension));
			}
		}
		return glfwExtensions;
	}

	VulkanInitializer* VulkanInitializer::prepareInstance() {
#if DEBUG
		auto validationLayers = this->requestValidationLayers();
		std::cout << "Debug Mode!! Requesting validation layers" << std::endl;
#endif // DEBUG

		auto glfwExtensions = this->getCompitableGlfwExtensions();
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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();
#if DEBUG
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif // !NDEBUG

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