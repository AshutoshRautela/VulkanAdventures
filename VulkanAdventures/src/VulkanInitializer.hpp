#pragma once

#define GLFW_INCLUDE_VULKAN
#define DEBUG !NDEBUG

#include <GLFW\glfw3.h>
#include <utility>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace va {
	class VulkanInitializer
	{
	public:
		VulkanInitializer(std::string, std::string);
		~VulkanInitializer();

		//Removing the copy constructor
		VulkanInitializer(const VulkanInitializer&) = delete;
		//Removing the assigment overloader operator
		const VulkanInitializer& operator=(const VulkanInitializer&) = delete;

	private:
		std::string applicationName;
		std::string engineName;

		VkInstance _vkInstance;

		VulkanInitializer* prepareInstance();
		std::vector<const char*> getCompitableGlfwExtensions();
		std::vector<const char*> requestValidationLayers();

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