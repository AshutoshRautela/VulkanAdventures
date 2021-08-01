#pragma once

#define GLFW_INCLUDE_VULKAN
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
		VulkanInitializer(std::string, std::string) throw (std::runtime_error);
		~VulkanInitializer();

		//Removing the copy constructor
		VulkanInitializer(const VulkanInitializer&) = delete;
		//Removing the assigment overloader operator
		const VulkanInitializer& operator=(const VulkanInitializer&) = delete;

	private:
		std::string applicationName;
		std::string engineName;

		VkInstance _vkInstance;
		VulkanInitializer* prepareInstance() throw (std::runtime_error);

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
		inline std::pair<uint32_t, const char**> getGlfwExtensions() {
			uint32_t glfwExtensionsCount{ 0 };
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
			return std::make_pair(glfwExtensionsCount, glfwExtensions);
		}
	};
}