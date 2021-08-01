#pragma once
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <string>

namespace va {
	class VAWindow
	{
	public:
		VAWindow(uint32_t, uint32_t, std::string) throw(std::runtime_error);
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



