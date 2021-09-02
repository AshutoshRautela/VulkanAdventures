#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)
#include <glm\glm.hpp>
#include <array>

namespace va {
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription vkVertexInputBindingDescription{};
			vkVertexInputBindingDescription.binding = 0;
			vkVertexInputBindingDescription.stride = sizeof(Vertex);
			vkVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return vkVertexInputBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescription{};

			attributeDescription[0].binding = 0;
			attributeDescription[0].location = 0;
			attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescription[0].offset = offsetof(Vertex, position);

			attributeDescription[1].binding = 0;
			attributeDescription[1].location = 1;
			attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescription[1].offset = offsetof(Vertex, color);

			return attributeDescription;
		}
	};
}

