#pragma once
#include <vector>

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include "../Logger.h"
#include "../Utils.hpp"

namespace va {
	class ValidationManager
	{
	private:
		std::vector<const char*> requiredValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

		VkDebugUtilsMessengerEXT _debugUtilsMessenger;
		VkDebugUtilsMessengerCreateInfoEXT _messengerCreateinfo{};
		VkInstance _vkInstance;
	public:
		ValidationManager();
		~ValidationManager();

		bool checkValidationSupport();
		void setupDebugMessenger(const VkInstance& vkInstance);

		inline VkDebugUtilsMessengerCreateInfoEXT* getDebugCreateInfo() {
			return &this->_messengerCreateinfo; 
		};
		inline std::vector<const char*> RequiredValidationLayers() const { return ValidationManager::requiredValidationLayers; }
	};
}



