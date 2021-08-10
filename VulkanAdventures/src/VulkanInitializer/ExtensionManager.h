#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>
#include <algorithm>
#include <iostream>

#include "../Logger.h"

namespace va {
	class ExtensionManager
	{
	public:
		static std::pair<uint32_t, const char**> getGLFWExtensions();
	};
}



