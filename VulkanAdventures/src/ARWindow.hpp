#pragma once

#pragma warning(push, 0)
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <string>
#include ".\Logger.h"

namespace va {
	class VAWindow
	{
	public:
		VAWindow(uint32_t, uint32_t, std::string);
		void PollEvents();
		~VAWindow();

		//Removing the copy constructor
		VAWindow(VAWindow&) = delete;
		//Removing the assigment overloader operator
		VAWindow& operator=(VAWindow&) = delete;

		inline GLFWwindow* GetRawWindow() { return this->glfwWindow; }

	private:
		uint32_t width;
		uint32_t height;
		std::string title;

		GLFWwindow* glfwWindow;
	};
}



