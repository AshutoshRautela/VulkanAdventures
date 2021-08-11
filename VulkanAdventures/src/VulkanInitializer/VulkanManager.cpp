#include "VulkanManager.h"

namespace va {
	VulkanManager::VulkanManager(const std::shared_ptr<VAWindow>& vaWindow, std::string appName, std::string engineName) {
		this->_extensionManager = std::make_unique<ExtensionManager>();
		auto extensionSupport = this->_extensionManager->isRequiredExtensionsSupported();
		if (!extensionSupport) {
			throw std::runtime_error("Extensions are not supported");
		}
#if _DEBUG
		this->_validationManager = std::make_unique<ValidationManager>();
		auto res = this->_validationManager->checkValidationSupport();
		if (!res) {
			this->_instanceManager = 
			std::make_unique<VulkanInstanceManager>(
				appName,
				engineName,
				this->_extensionManager->getInstanceRequiredExtensions()
			);
			LOGGER_WARN("Validation Layers are not supported! Created Instance without Validation Layers!");
		}
		else {
			this->_instanceManager =
				std::make_unique<VulkanInstanceManager>(
					appName,
					engineName,
					this->_extensionManager->getInstanceRequiredExtensions(),
					this->_validationManager->RequiredValidationLayers(),
					this->_validationManager->getDebugCreateInfo()
				);
		}
		this->_validationManager->setupDebugMessenger(this->_instanceManager->getVkInstance());
#endif
#if _RELEASE
		this->_instanceManager =
			std::make_unique<VulkanInstanceManager>(
				appName,
				engineName,
				this->_extensionManager->getInstanceRequiredExtensions()
			);
#endif
		this->_instanceManager->createSurface(vaWindow->GetRawWindow());
		this->_queueManager = std::make_shared<QueueManager>();
		this->_swapchainManager = std::make_shared<SwapchainManager>();
		this->_deviceManager = std::make_unique<DeviceManager>(this->_instanceManager->getVkInstance(), this->_queueManager, this->_swapchainManager);
		this->_deviceManager->pickPhysicalDevice(this->_instanceManager->getWindowSurface());
#if _DEBUG
		this->_deviceManager->createLogicalDevice(this->_extensionManager->getInstanceRequiredExtensions(), this->_validationManager->RequiredValidationLayers());
#endif
#if _RELEASE
		this->_deviceManager->createLogicalDevice(this->_extensionManager->getInstanceRequiredExtensions());
#endif
		this->_queueManager->UpdateQueues(this->_deviceManager->getLogicalDevice());
		this->_swapchainManager->createSwapChain(
			vaWindow->GetRawWindow(),
			this->_deviceManager->getPhysicalDevice(),
			this->_deviceManager->getLogicalDevice(),
			this->_instanceManager->getWindowSurface(),
			this->_queueManager
		);
		this->_swapchainManager->createImageViews(this->_deviceManager->getLogicalDevice());
	}
}