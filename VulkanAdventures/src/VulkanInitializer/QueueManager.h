#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <optional>
#include <vector>

#include "../Logger.h"

namespace va {

	struct QueueFamilyIndices {

		std::optional<uint32_t> graphisFamily;
		std::optional<uint32_t> presentFamily;
		float queuePriority = 1.0f;

		inline bool isAvailable() { return this->graphisFamily.has_value() && this->presentFamily.has_value(); }
	};

	class QueueManager
	{
		QueueFamilyIndices _queueFamilyIndices;

		VkQueue _graphicsQueue;
		VkQueue _presentQueue;

	public:
		inline const QueueFamilyIndices& getQueueFamilyIndices() { return this->_queueFamilyIndices;  }
		bool checkAvailableQueueFamilies(const VkPhysicalDevice&, const VkSurfaceKHR&);

		inline const VkQueue& getGraphicsQueue() const { return this->_graphicsQueue; }
		inline const VkQueue& getPresentQueue() const { return this->_presentQueue; }

		void UpdateQueues(const VkDevice&);
	};

}



