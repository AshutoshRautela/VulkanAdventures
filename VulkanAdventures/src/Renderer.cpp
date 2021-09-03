#include "Renderer.h"

namespace va {

	Renderer::Renderer(const VkPhysicalDevice& vkPhysicalDevice, const VkDevice& vkDevice, Mesh&& mesh):_vkPhysicalDevice(vkPhysicalDevice), _vkDevice(vkDevice) {
		this->_mesh = std::make_unique<Mesh>(mesh);
	}

	void Renderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		this->_vkBufferCreateInfo = {};
		this->_vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		this->_vkBufferCreateInfo.size = size;
		this->_vkBufferCreateInfo.usage = usage;
		this->_vkBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->_vkDevice, &this->_vkBufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
#if DEBUG
			LOGGER_ERROR("Failed to create buffer");
#endif // DEBUG
			throw std::runtime_error("Failed to create buffer");
		}

#if DEBUG
		LOGGER_INFO("Successfully created the Buffer");
#endif // DEBUG

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->_vkDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo vkMemoryAllocateInfo{};
		vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vkMemoryAllocateInfo.allocationSize = memRequirements.size;
		vkMemoryAllocateInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(this->_vkDevice, &vkMemoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
#if DEBUG
			LOGGER_ERROR("Failed to allocated memory for the buffer");
#endif // DEBUG
			throw std::runtime_error("Failed to allocate memory for the buffer");
		}

		vkBindBufferMemory(this->_vkDevice, buffer, bufferMemory, 0);
	}

	void Renderer::createVertexBuffers(const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {
		VkDeviceSize bufferSize = sizeof(this->_mesh->getVertices()[0]) * this->_mesh->getVertices().size();

		VkBuffer stagingBuffer; // Staging Buffer
		VkDeviceMemory stagingBufferMemory; // Staging Buffer Memory
		
		this->createBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		// Copy data to Staging Buffer
		void* data;
		vkMapMemory(this->_vkDevice, stagingBufferMemory, 0, this->_vkBufferCreateInfo.size, 0, &data);
		memcpy(data, this->_mesh->getVertices().data(), (size_t)this->_vkBufferCreateInfo.size);
		vkUnmapMemory(this->_vkDevice, stagingBufferMemory);
#if DEBUG
		LOGGER_INFO("Copied data to the Staging Buffer");
#endif // DEBUG

		this->createBuffer(bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			this->_vertexBuffer,
			this->_vertexBufferMemory
		);

		this->copyBuffer(commandPool, graphicsQueue, stagingBuffer, this->_vertexBuffer, bufferSize);
#if DEBUG
		LOGGER_INFO("Sccessfully copied data to the Vertex buffer");
#endif // DEBUG

		// Cleaning Staging Buffers after copying data to Vertex Buffer
		vkDestroyBuffer(this->_vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(this->_vkDevice, stagingBufferMemory, nullptr);

		this->createIndexBuffers(commandPool, graphicsQueue);
	}

	void Renderer::createIndexBuffers(const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {
		VkDeviceSize bufferSize = sizeof(this->_mesh->getIndices()[0]) * this->_mesh->getIndices().size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		
		this->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(this->_vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->_mesh->getIndices().data(), (size_t)bufferSize);
		vkUnmapMemory(this->_vkDevice, stagingBufferMemory);

		this->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->_indexBuffer, this->_indexBufferMemory);

		this->copyBuffer(commandPool, graphicsQueue, stagingBuffer, this->_indexBuffer, bufferSize);

		vkDestroyBuffer(this->_vkDevice, stagingBuffer, nullptr);
		vkFreeMemory(this->_vkDevice, stagingBufferMemory, nullptr);

#if DEBUG
		LOGGER_INFO("Successfully copied data to Index Buffer");
#endif // DEBUG

	}

	void Renderer::copyBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcbuffer, VkBuffer destBuffer, VkDeviceSize size) {
		VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
		vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vkCommandBufferAllocateInfo.commandPool = commandPool;
		vkCommandBufferAllocateInfo.commandBufferCount = 1;

		VkCommandBuffer vkCommandBuffer;
		vkAllocateCommandBuffers(this->_vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(vkCommandBuffer, srcbuffer, destBuffer, 1, &copyRegion);

		vkEndCommandBuffer(vkCommandBuffer);

		VkSubmitInfo vkSubmitInfo{};
		vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		vkSubmitInfo.commandBufferCount = 1;
		vkSubmitInfo.pCommandBuffers = &vkCommandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(this->_vkDevice, commandPool, 1, &vkCommandBuffer);
	}

	void Renderer::render(const VkCommandBuffer& commandBuffer) {
		VkBuffer vertexBuffers[] = { this->_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, this->_indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(this->_mesh->getIndices().size()), 1, 0, 0, 0);
		//vkCmdDraw(commandBuffer, static_cast<uint32_t>(this->_mesh->getVertices().size()), 1 , 0 , 0);
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
		vkDestroyBuffer(this->_vkDevice, this->_indexBuffer, nullptr);
		vkFreeMemory(this->_vkDevice, this->_indexBufferMemory, nullptr);

		vkDestroyBuffer(this->_vkDevice, this->_vertexBuffer, nullptr);
		vkFreeMemory(this->_vkDevice, this->_vertexBufferMemory, nullptr);
	}
}