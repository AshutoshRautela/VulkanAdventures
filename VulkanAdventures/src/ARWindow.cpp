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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
		if (!this->glfwWindow) {
			throw std::runtime_error("Failed to create GLFW Window");
		}
#if _DEBUG
		LOGGER_INFO("GLFW Window created");
#endif
		glfwSetWindowUserPointer(this->glfwWindow, reinterpret_cast<VAWindow*>(this));
		glfwSetFramebufferSizeCallback(this->glfwWindow, [](GLFWwindow* window, int width, int height)->void {
			VAWindow* vaWindow = static_cast<VAWindow*>(glfwGetWindowUserPointer(window));
			if (vaWindow != nullptr) {
				vaWindow->resizeWindowCallback(width, height);
			}
#if _DEBUG
			else {
				LOGGER_ERROR("GLFW Window cannot find user pointer");
			}
#endif
		});
	}

	void VAWindow::resizeWindowCallback(uint32_t width, uint32_t height) {
		this->width = width; this->height = height;
		for (auto x : this->_windowResizeCallbacks) {
			x(this->width, this->height);
		}
	}

	void VAWindow::onWindowResize(windowResizeCallback callback) {
		this->_windowResizeCallbacks.push_back(callback);
		LOGGER_INFO("Callbacks: {0}", this->_windowResizeCallbacks.size());
	}

	void VAWindow::PollEvents() {
		glfwPollEvents();
	}

	VAWindow::~VAWindow() {
		this->_windowResizeCallbacks.clear();
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