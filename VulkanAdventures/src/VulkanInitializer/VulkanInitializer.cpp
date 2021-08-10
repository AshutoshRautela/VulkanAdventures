#include "VulkanInitializer.hpp"
#include <set>

namespace va {
	VulkanInitializer::VulkanInitializer(GLFWwindow* window, std::string appName, std::string engineName) : window { window }, applicationName { appName }, engineName{ engineName } {
		this->_vkInstance = nullptr;
		this->prepareInstance();
#if DEBUG
		this->_createMessengerinfo = {};
		this->_vkDebugMessager = nullptr;
		this->prepareDebugMessenger();
#endif // 0
		this->createWindowSurface();
		this->preparePhysicalDevice()
			->prepareLogicalDevice();
	}

#if DEBUG
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
		this->_validationLayers = requiredvalidationLayers;
		return this->_validationLayers;
	}
#endif

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

	VulkanInitializer* VulkanInitializer::createWindowSurface() {
		if (glfwCreateWindowSurface(this->_vkInstance, this->window, nullptr, &this->_vkSurfaceKHR) != VK_SUCCESS) {
			throw std::runtime_error("Failed to created Window Surface");
		}	
		std::cout << "Created Window Surface Successfully" << std::endl;
	}

	VulkanInitializer* VulkanInitializer::preparePhysicalDevice() {
		std::cout << "Founding a suitable physical device " << std::endl;
		std::vector<VkPhysicalDevice> physicalDevices = this->getPhysicalDevices();
		VkPhysicalDevice device = this->getSuitableDevice(physicalDevices);

		if (device == VK_NULL_HANDLE) {
			throw std::runtime_error("Couldn't find a suitable physical device");
		}

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		std::cout << "Found suitable physical device: " << properties.deviceName << std::endl;
		this->_vkPhysicalDevice = device;
		return this;
	}

	VulkanInitializer* VulkanInitializer::prepareLogicalDevice() {
		std::cout << "Creating a Logical Device " << std::endl;
		float queuePriorities = 1.0;

		std::vector<VkDeviceQueueCreateInfo> queueCreateDeviceInfos;
		std::set<int32_t> uniqueQueueFamiles = { this->_queueFamilyIndices.graphicsFamily, this->_queueFamilyIndices.presentFamily };

		for (int32_t queueFamily : uniqueQueueFamiles) {
			VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo{};
			vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vkDeviceQueueCreateInfo.queueFamilyIndex = queueFamily;
			vkDeviceQueueCreateInfo.queueCount = 1;
			vkDeviceQueueCreateInfo.pQueuePriorities = &queuePriorities;

			queueCreateDeviceInfos.push_back(vkDeviceQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures{};

		VkDeviceCreateInfo vkDeviceCreateInfo{};
		vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vkDeviceCreateInfo.pQueueCreateInfos = queueCreateDeviceInfos.data();
		vkDeviceCreateInfo.queueCreateInfoCount = queueCreateDeviceInfos.size();
		vkDeviceCreateInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
		vkDeviceCreateInfo.enabledExtensionCount = this->extensions.size();
		vkDeviceCreateInfo.ppEnabledExtensionNames = this->extensions.data();

#if DEBUG
		vkDeviceCreateInfo.enabledLayerCount = this->_validationLayers.size();
		vkDeviceCreateInfo.ppEnabledLayerNames = this->_validationLayers.data();
#endif // DEBUG
		vkDeviceCreateInfo.enabledLayerCount = 0;
		
		if (vkCreateDevice(this->_vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &this->_vkLogicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Logical Device");
		}
		std::cout << "Created Logical device successfully" << std::endl;
		vkGetDeviceQueue(this->_vkLogicalDevice, this->_queueFamilyIndices.graphicsFamily, 0, &this->_vkGraphicsQueue );
		if (this->_vkGraphicsQueue == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to fetch queue from Logical Device");
		}
		std::cout << "Fetched graphics queue handle from the Logical Device" << std::endl;

		vkGetDeviceQueue(this->_vkLogicalDevice, this->_queueFamilyIndices.presentFamily, 0, &this->_vkPresentQueue);
		if (this->_vkPresentQueue == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to fetch present queue from Logical Device");
		}
		std::cout << "Fetched present queue handle from the Logical Device" << std::endl;
		return this;
	}

	VulkanInitializer::~VulkanInitializer() {
		vkDestroyDevice(this->_vkLogicalDevice, nullptr);
		vkDestroySurfaceKHR(this->_vkInstance, this->_vkSurfaceKHR, nullptr);
#if DEBUG
		this->_createMessengerinfo = {};
		va::vkDestroyDebugUtilsMessengerEXT(this->_vkInstance, this->_vkDebugMessager, nullptr);
#endif
		vkDestroyInstance(this->_vkInstance, nullptr);
	}
}