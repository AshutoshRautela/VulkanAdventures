#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#define DEBUG !NDEBUG

#include <utility>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

#include "Utils.hpp"

namespace va {
	class VulkanInitializer
	{
	public:
		VulkanInitializer(std::string, std::string);
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
		std::string applicationName;
		std::string engineName;

		VkInstance _vkInstance;
#if DEBUG
		VkDebugUtilsMessengerCreateInfoEXT _createMessengerinfo;
		VkDebugUtilsMessengerEXT _vkDebugMessager;
#endif // DEBUG

		std::vector<const char*> getCompitableGlfwExtensions();
		std::vector<const char*> requestValidationLayers();

		// Initialization Process
		VulkanInitializer* prepareInstance();

#if DEBUG
		VulkanInitializer* prepareDebugMessenger();
#endif // DEBUG

		inline void createDebugMessengerInfo() {
			this->_createMessengerinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			this->_createMessengerinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			this->_createMessengerinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			this->_createMessengerinfo.pfnUserCallback = VulkanInitializer::debugCallback;
		}

		// Getting details of supported Vulkan Extensions
		inline std::vector<VkExtensionProperties> getSupportVkExtensions() {
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
		inline std::vector<const char*> getGlfwExtensions() {
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
		inline std::vector<VkLayerProperties> getValidationLayers() {
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