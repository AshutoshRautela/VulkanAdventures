#pragma once

#pragma warning(push, 0)
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <string>
#include <vector>
#include ".\Logger.h"

namespace va {
	typedef std::function<void(uint32_t, uint32_t)> windowResizeCallback;

	class VAWindow
	{
	public:
		VAWindow(uint32_t, uint32_t, std::string);
		void PollEvents();
		~VAWindow();

		//Removing the copy constructor
		VAWindow(VAWindow&) = delete;
		//Removing the assigment overloader operator
		VAWindow& operator=(VAWindow& win) = delete;

		inline GLFWwindow* GetRawWindow() { return this->glfwWindow; }
		void resizeWindowCallback(uint32_t, uint32_t);
		void onWindowResize(windowResizeCallback);

	private:
		uint32_t width;
		uint32_t height;
		std::string title;
		
		std::vector<windowResizeCallback> _windowResizeCallbacks;

		GLFWwindow* glfwWindow;
	};
}



