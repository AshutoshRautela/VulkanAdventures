#include "ExtensionManager.h"

namespace va {
	std::pair<uint32_t, const char**> ExtensionManager::getGLFWExtensions() {
		uint32_t glfwExtensionsCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
		
		LOGGER_INFO("GLFW REQUIRED EXTENSIONS");

		for (int i = 0; i < glfwExtensionsCount; i++) {
			//std::cout << glfwExtensions[i] << std::endl;
			LOGGER_INFO(glfwExtensions[i]);
		}

		return std::make_pair(glfwExtensionsCount, glfwExtensions);
	}
}