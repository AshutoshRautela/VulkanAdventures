#include "ValidationManager.h"

namespace va {

	ValidationManager::ValidationManager() {
		this->_messengerCreateinfo = {};
		this->_messengerCreateinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		this->_messengerCreateinfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		this->_messengerCreateinfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		this->_messengerCreateinfo.pfnUserCallback = ValidationManager::debugCallback;
		this->_messengerCreateinfo.pUserData = nullptr;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL ValidationManager::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOGGER_ERROR("Validation Error: {0}", pCallbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT || VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			//LOGGER_INFO("Validation Info: {0}", pCallbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOGGER_WARN("Validation Warning: {0}", pCallbackData->pMessage);
			break;

		default:
			//LOGGER_INFO("Validation Unknown: {0}", pCallbackData->pMessage);
			break;
		}

		return VK_FALSE;
	}

	void ValidationManager::setupDebugMessenger(const VkInstance& vkInstance) {
		this->_vkInstance = vkInstance;

		VkResult res = va::vkCreateDebugUtilsMessengerEXT(vkInstance, &this->_messengerCreateinfo, nullptr, &this->_debugUtilsMessenger);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Debug utils Messenger");
		}

#if _DEBUG
		LOGGER_INFO("Successfully created Debug Utils Messenger");
#endif
	}

	bool ValidationManager::checkValidationSupport() {
		uint32_t validationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

		std::vector<VkLayerProperties> validationLayerProperties(validationLayerCount);
		vkEnumerateInstanceLayerProperties(&validationLayerCount, validationLayerProperties.data());

#if _DEBUG
		LOGGER_INFO("\n----->Available Validation Layers<-----\n");
		for (auto& vl : validationLayerProperties) {
			LOGGER_INFO(vl.layerName);
		}
		LOGGER_INFO("\n----------\n");
#endif

		for (auto& rvl : this->requiredValidationLayers) {
			bool found = false;
			for (auto& avl : validationLayerProperties) {
				if (strcmp(avl.layerName, rvl) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				LOGGER_ERROR("Validation Layer not supported: {0}", rvl);
				return false;
			}
		}

#if _DEBUG
		LOGGER_INFO("Validation layers are supported");
		for (auto& vl : this->requiredValidationLayers) {
			LOGGER_INFO(vl);
		}
#endif // _DEBUG
		return true;
	}

	ValidationManager::~ValidationManager() {
		if (this->_debugUtilsMessenger != VK_NULL_HANDLE) {
			va::vkDestroyDebugUtilsMessengerEXT(this->_vkInstance, this->_debugUtilsMessenger, nullptr);
		}
	}
}