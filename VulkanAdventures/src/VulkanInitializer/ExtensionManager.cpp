#include "ExtensionManager.h"

namespace va {

	std::vector<const char*> ExtensionManager::getGlfwExtensions() {
		uint32_t glfwExtensionsCount = 0;

		// GLFW Extensions
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
		
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionsCount);;
	}

	std::vector<const char*> ExtensionManager::getInstanceRequiredExtensions() {
		if (this->_requiredExtensions.size() == 0) {
			this->_requiredExtensions = this->getGlfwExtensions();
#if _DEBUG
			// Validation Extension
			this->_requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			LOGGER_INFO("\n----->REQUIRED EXTENSIONS<-----\n");
			for (auto& ex : this->_requiredExtensions) {
				LOGGER_INFO(ex);
			}
			LOGGER_INFO("\n----------\n");
#endif
		}
		return _requiredExtensions;
	}

	std::vector<VkExtensionProperties> ExtensionManager::getInstanceAvailableExtensions() {
		if (this->_availableExtensions.size() == 0) {
			uint32_t availableExtensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

			this->_availableExtensions = std::vector<VkExtensionProperties>(availableExtensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, this->_availableExtensions.data());

#if _DEBUG
			LOGGER_INFO("\n----->AVAILABLE EXTENSIONS<-----\n");
			for (auto& ex : this->_availableExtensions) {
				LOGGER_INFO(ex.extensionName);
			}
			LOGGER_INFO("\n----------\n");
#endif
		}
		return this->_availableExtensions;
	}

	bool ExtensionManager::isRequiredExtensionsSupported() {
		this->getInstanceRequiredExtensions();
		this->getInstanceAvailableExtensions();

		for (auto& rex : this->_requiredExtensions) {
			bool found = false;
			for (auto& aex : this->_availableExtensions) {
				if (strcmp(rex, aex.extensionName) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				LOGGER_WARN("Extension not supported {0}", rex);
				return false;
			}
		}

#if _DEBUG
		LOGGER_INFO("All Extensions are supported!");
#endif
		return true;
	}

	ExtensionManager::~ExtensionManager() {
		this->_requiredExtensions.clear();
		this->_availableExtensions.clear();
	}
}