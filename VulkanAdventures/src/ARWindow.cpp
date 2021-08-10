#include <iostream>
#include "ARWindow.hpp"

namespace va {
	VAWindow::VAWindow(uint32_t width, uint32_t height, std::string title) : width{ width }, height{ height }, title{ title }, glfwWindow{ nullptr } {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to Initialize GLFW");
		}
		LOGGER_INFO("GLFW Initialized");

		// Specifically mention not to create OpenGL Context for the window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
		if (!this->glfwWindow) {
			throw std::runtime_error("Failed to create GLFW Window");
		}
		LOGGER_INFO("GLFW Window created");
	}

	void VAWindow::PollEvents() {
		glfwPollEvents();
	}

	VAWindow::~VAWindow() {
		glfwDestroyWindow(this->glfwWindow);
		LOGGER_INFO("GLFW Window Destroyed");
		glfwTerminate();
		LOGGER_INFO("GLFW Terminated");
	}
}