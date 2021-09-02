#include "Renderer.h"

namespace va {

	Renderer::Renderer(const VkPhysicalDevice& vkPhysicalDevice, const VkDevice& vkDevice, std::vector<Vertex> vertices):_vkPhysicalDevice(vkPhysicalDevice), _vkDevice(vkDevice) {
		this->_mesh = std::make_unique<Mesh>(vertices);
		this->createVertexBuffers();
		this->allocateMemoryForVertexBuffers();
		this->copyDataToVertexBuffer();
	}

	void Renderer::createVertexBuffers() {
		this->_vkBufferCreateInfo = {};
		this->_vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		this->_vkBufferCreateInfo.size = sizeof(this->_mesh->getVertices()[0]) * this->_mesh->getVertices().size();
		this->_vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		this->_vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->_vkDevice, &this->_vkBufferCreateInfo, nullptr, &this->_vertexBuffer) != VK_SUCCESS) {
#if DEBUG
			LOGGER_INFO("Failed to create Vertex Buffer");
#endif // DEBUG
			throw std::runtime_error("Failed to create Vetex Buffer");
		}
#if DEBUG
		LOGGER_INFO("Successfully created the Vertex Buffer");
#endif // DEBUG
	}

	void Renderer::allocateMemoryForVertexBuffers() {
		VkMemoryRequirements vkMemoryRequirements{};
		vkGetBufferMemoryRequirements(this->_vkDevice, this->_vertexBuffer, &vkMemoryRequirements);

		VkMemoryAllocateInfo vkMemoryAllocateInfo{};
		vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
		vkMemoryAllocateInfo.memoryTypeIndex = this->findMemoryType(vkMemoryRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(this->_vkDevice, &vkMemoryAllocateInfo, nullptr, &this->_vertexBufferMemory) != VK_SUCCESS) {
#if DEBUG
			LOGGER_ERROR("Failed to allocated memory for Vertex Buffer");
#endif // DEBUG
			throw std::runtime_error("Failed to allocate memory for Vertex Buffer");
		}
		vkBindBufferMemory(this->_vkDevice, this->_vertexBuffer, this->_vertexBufferMemory, 0);
#if DEBUG
		LOGGER_INFO("Successfully allocated memory for Vertex Buffer");
#endif
	}

	void Renderer::copyDataToVertexBuffer() {
		void* data;
		vkMapMemory(this->_vkDevice, this->_vertexBufferMemory, 0, this->_vkBufferCreateInfo.size, 0, &data);
		memcpy(data, this->_mesh->getVertices().data(), (size_t)this->_vkBufferCreateInfo.size);
		vkUnmapMemory(this->_vkDevice, this->_vertexBufferMemory);
#if DEBUG
		LOGGER_INFO("Copied data to the Vertex Buffer");
#endif // DEBUG

	}

	void Renderer::render(const VkCommandBuffer& commandBuffer) {
		VkBuffer vertexBuffers[] = { this->_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdDraw(commandBuffer, static_cast<uint32_t>(this->_mesh->getVertices().size()), 1 , 0 , 0);
	}

	uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(this->_vkPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	Renderer::~Renderer() {
		vkDestroyBuffer(this->_vkDevice, this->_vertexBuffer, nullptr);
		vkFreeMemory(this->_vkDevice, this->_vertexBufferMemory, nullptr);
	}
}