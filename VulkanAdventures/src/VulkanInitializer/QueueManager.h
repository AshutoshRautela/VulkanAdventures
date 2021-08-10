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
		float queuePriority = 1.0f;

		inline bool isAvailable() { return this->graphisFamily.has_value(); }
	};

	class QueueManager
	{
		QueueFamilyIndices _queueFamilyIndices;

		VkQueue _graphicsQueue;

	public:
		inline const QueueFamilyIndices& getQueueFamilyIndices() { return this->_queueFamilyIndices;  }
		bool checkAvailableQueueFamilies(const VkPhysicalDevice&);

		void UpdateQueues(const VkDevice&);
	};

}


