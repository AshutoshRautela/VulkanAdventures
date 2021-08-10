#pragma once

#include "./VulkanInstanceManager.h"
#include "./ValidationManager.h"
#include "./ExtensionManager.h"

namespace va {
	class VulkanManager
	{
	public:
		VulkanManager(std::string, std::string);

		VulkanManager(const VulkanManager&) = delete;
		const VulkanManager& operator=(const VulkanManager&) = delete;

	private:
		std::unique_ptr<VulkanInstanceManager> _instanceManager;

		std::unique_ptr<ExtensionManager> _extensionManager;
		std::unique_ptr<ValidationManager> _validationManager;
	};
}



