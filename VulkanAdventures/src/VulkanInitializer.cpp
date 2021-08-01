#include "VulkanInitializer.hpp"

namespace va {
	VulkanInitializer::VulkanInitializer(std::string appName, std::string engineName): applicationName{ appName }, engineName{engineName} {
		this->_vkInstance = nullptr;
#if DEBUG
		this->_createMessengerinfo = {};
		this->_vkDebugMessager = nullptr;
		this->prepareInstance();
#endif // 0
#if DEBUG
		this->prepareDebugMessenger();
#endif
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInitializer::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbacksData,
		void* pUserData
	) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			std::cout << "Validation Layer: " << pCallbacksData->pMessage << std::endl;
		}
		return VK_FALSE;
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
		for (const char* vLayer : validationLayers) {
			std::cout << "Validation Layers " << vLayer << std::endl;
		}
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
		this->createDebugMessengerInfo();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&this->_createMessengerinfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif // !NDEBUG

		createInfo.enabledLayerCount = 0;
		if (vkCreateInstance(&createInfo, nullptr, &this->_vkInstance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan Instance");
		}
		std::cout << "Created Vulkan Instance Successfully" << std::endl;
		return this;
	}

#if DEBUG
	VulkanInitializer* VulkanInitializer::prepareDebugMessenger() {
		if (va::vkCreateDebugUtilsMessengerEXT(this->_vkInstance, &this->_createMessengerinfo, nullptr, &this->_vkDebugMessager) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Debug Messenger Util");
		}
		std::cout << "Created Debug Messenger Successfully" << std::endl;
		return this;
	}
#endif // DEBUG

	VulkanInitializer::~VulkanInitializer() {
#if DEBUG
		this->_createMessengerinfo = {};
		if (this->_vkDebugMessager) {
		//	va::vkDestroyDebugUtilsMessengerEXT(this->_vkInstance, this->_vkDebugMessager, nullptr);
		}
#endif
		if (this->_vkInstance) {
		//	vkDestroyInstance(this->_vkInstance, nullptr);
		}
	}
}