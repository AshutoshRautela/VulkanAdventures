#include <iostream>
#include "ARWindow.hpp";

namespace va {
	VAWindow::VAWindow(uint32_t width, uint32_t height, std::string title) throw(std::runtime_error) : width{ width }, height{ height }, title{ title }, glfwWindow{ nullptr } {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to Initialize GLFW");
		}
		std::cout << "GLFW scuccessfully Initialized" << std::endl;

		// Specifically mention not to create OpenGL Context for the window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
		if (!this->glfwWindow) {
			throw std::runtime_error("Failed to create GLFW Window");
		}
		std::cout << "GFLW window successfully created" << std::endl;
	}

	void VAWindow::PollEvents() {
		glfwPollEvents();
	}

	VAWindow::~VAWindow() {
		if (this->glfwWindow != nullptr) {
			glfwDestroyWindow(this->glfwWindow);
		}
		glfwTerminate();
	}
}