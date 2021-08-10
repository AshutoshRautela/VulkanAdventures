#include <iostream>
#include "ARWindow.hpp"

namespace va {
	VAWindow::VAWindow(uint32_t width, uint32_t height, std::string title) : width{ width }, height{ height }, title{ title }, glfwWindow{ nullptr } {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to Initialize GLFW");
		}
#if _DEBUG
		LOGGER_INFO("GLFW Initialized");
#endif

		// Specifically mention not to create OpenGL Context for the window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
		if (!this->glfwWindow) {
			throw std::runtime_error("Failed to create GLFW Window");
		}
#if _DEBUG
		LOGGER_INFO("GLFW Window created");
#endif
	}

	void VAWindow::PollEvents() {
		glfwPollEvents();
	}

	VAWindow::~VAWindow() {
		glfwDestroyWindow(this->glfwWindow);
#if _DEBUG
		LOGGER_INFO("GLFW Window Destroyed");
#endif
		glfwTerminate();
#if _DEBUG
		LOGGER_INFO("GLFW Terminated");
#endif
	}
}