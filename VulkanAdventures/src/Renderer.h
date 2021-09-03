#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)
#include <memory>

#include "Mesh.h"
#include "Logger.h"

namespace va {

	class Renderer
	{
	public:
		Renderer(const VkPhysicalDevice&, const VkDevice&, std::vector<Vertex>);
		void createVertexBuffers(const VkCommandPool&, const VkQueue&);
		
		void render(const VkCommandBuffer&);
		~Renderer();

	private:
		void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
		
		void copyBuffer(const VkCommandPool&, const VkQueue&, VkBuffer, VkBuffer, VkDeviceSize);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		const VkPhysicalDevice& _vkPhysicalDevice;
		const VkDevice& _vkDevice;
		std::unique_ptr<Mesh> _mesh;

		VkBufferCreateInfo _vkBufferCreateInfo{};

		VkBuffer _stagingBuffer; // Staging Buffer
		VkDeviceMemory _stagingBufferMemory; // Staging Buffer Memory

		VkBuffer _vertexBuffer; // Vertex Buffer
		VkDeviceMemory _vertexBufferMemory; //Vertex buffer Memory
	};
}