#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#define DEBUG !NDEBUG

#include <utility>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <optional>
#include <functional>
#include <set>

#include "..\Utils.hpp"
#include "..\ARWindow.hpp"

namespace va {
	class VulkanInitializer
	{
	public:
		VulkanInitializer(GLFWwindow* window, std::string, std::string);
		~VulkanInitializer();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT*,
			void*
		);

		//Removing the copy constructor
		VulkanInitializer(const VulkanInitializer&) = delete;
		//Removing the assigment overloader operator
		const VulkanInitializer& operator=(const VulkanInitializer&) = delete;

	private:
		struct QueueFamilyIndices
		{
			int32_t graphicsFamily = -1;
			int32_t presentFamily = -1;

			inline bool indicesFound() {
				return graphicsFamily >= 0 && presentFamily >= 0;
			}
		} _queueFamilyIndices;

		std::string applicationName;
		std::string engineName;
		GLFWwindow* window;

		std::vector<const char*> extensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance _vkInstance;
		VkSurfaceKHR _vkSurfaceKHR;
		VkPhysicalDevice _vkPhysicalDevice;
		VkDevice _vkLogicalDevice;
		VkQueue _vkGraphicsQueue;
		VkQueue _vkPresentQueue;

#if DEBUG
		VkDebugUtilsMessengerCreateInfoEXT _createMessengerinfo;
		VkDebugUtilsMessengerEXT _vkDebugMessager;
		std::vector<const char*> _validationLayers;

		VulkanInitializer* prepareDebugMessenger();
#endif // DEBUG

		std::vector<const char*> getCompitableGlfwExtensions();
		std::vector<const char*> requestValidationLayers();

		// Initialization Process
		VulkanInitializer* prepareInstance();
		// Create Window Surface
		VulkanInitializer* createWindowSurface();
		// Prepare Physical Device
		VulkanInitializer* preparePhysicalDevice();
		// Prepare Logical Device
		VulkanInitializer* prepareLogicalDevice();

#if DEBUG
		inline void createDebugMessengerInfo() {
			this->_createMessengerinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			this->_createMessengerinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			this->_createMessengerinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			this->_createMessengerinfo.pfnUserCallback = VulkanInitializer::debugCallback;
		}
#endif // DEBUG

		inline std::vector<VkPhysicalDevice> getPhysicalDevices() const {
			uint32_t physicalDeviceCount{ 0 };
			vkEnumeratePhysicalDevices(this->_vkInstance, &physicalDeviceCount, nullptr);

			if (physicalDeviceCount > 0) {
				std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
				vkEnumeratePhysicalDevices(this->_vkInstance, &physicalDeviceCount, physicalDevices.data());

				return physicalDevices;
			}
			return std::vector<VkPhysicalDevice>();
		}

		inline VkPhysicalDevice getSuitableDevice(const std::vector<VkPhysicalDevice>& devices) {
			VkPhysicalDevice suitableDevice = VK_NULL_HANDLE;

			for (const VkPhysicalDevice& device : devices) {
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(device, &properties);

				VkPhysicalDeviceFeatures features;
				vkGetPhysicalDeviceFeatures(device, &features);

				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader) {
					suitableDevice = device;
					break;
				}
			}

			if (suitableDevice == VK_NULL_HANDLE) {
				return suitableDevice;
			}

			// Checking for supported queue families
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(suitableDevice, &queueFamilyCount, nullptr);

			uint32_t i = 0;
			if (queueFamilyCount > 0 && suitableDevice != VK_NULL_HANDLE) {
				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(suitableDevice, &queueFamilyCount, queueFamilies.data());

				VkBool32 presentSupport = false;

				for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
					if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						this->_queueFamilyIndices.graphicsFamily = i;
					}
					if (!presentSupport) {
						vkGetPhysicalDeviceSurfaceSupportKHR(suitableDevice, i, this->_vkSurfaceKHR, &presentSupport);
						this->_queueFamilyIndices.presentFamily = i;
					}
					if (this->_queueFamilyIndices.indicesFound()) {
						break;
					}
					i++;
				}
			}
			if (!this->_queueFamilyIndices.indicesFound()) {
				suitableDevice = VK_NULL_HANDLE;
				return suitableDevice;
			}

			if (!this->checkDeviceExtensionSupport(suitableDevice)) {
				std::cout << "Physical Device doesn't support swap chain extension " << std::endl;
				suitableDevice = VK_NULL_HANDLE;
				return suitableDevice;
			}

			return suitableDevice;
		}

		inline bool checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
			uint32_t deviceExtensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtenions(deviceExtensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, availableExtenions.data());

			std::set<std::string> requiredExtensions(this->extensions.begin(), this->extensions.end());
			for (const auto& extension : availableExtenions) {
				requiredExtensions.erase(extension.extensionName);
			}
			return requiredExtensions.empty();
		}

		// Getting details of supported Vulkan Extensions
		inline std::vector<VkExtensionProperties> getSupportVkExtensions() const {
			uint32_t extensionsCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
			if (extensionsCount > 0) {
				std::vector<VkExtensionProperties> extensions(extensionsCount);
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());
				return extensions;
			}
			return std::vector<VkExtensionProperties>();
		}

		// Getting details of required extensions for Glfw
		inline std::vector<const char*> getGlfwExtensions() const {
			uint32_t glfwExtensionsCount{ 0 };
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

			std::vector<const char*> glfwExt(glfwExtensions, glfwExtensions + glfwExtensionsCount);
#if DEBUG
			glfwExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
			return glfwExt;
		}

		// Getting details of validation layers
		inline std::vector<VkLayerProperties> getValidationLayers() const {
			uint32_t validationLayersCount;
			vkEnumerateInstanceLayerProperties(&validationLayersCount, nullptr);
			if (validationLayersCount > 0) {
				std::vector<VkLayerProperties> validationLayers(validationLayersCount);
				vkEnumerateInstanceLayerProperties(&validationLayersCount, validationLayers.data());
				return validationLayers;
			}
			return std::vector<VkLayerProperties>();
		}
	};
}