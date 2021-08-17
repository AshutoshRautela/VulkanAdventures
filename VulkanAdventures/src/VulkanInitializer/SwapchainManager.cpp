#include "SwapchainManager.h"

namespace va {

	SwapchainManager::SwapchainManager() {}

	bool SwapchainManager::checkSwapchainExtensionsSupport(const VkPhysicalDevice& vkPhysicalDevice) {

		uint32_t availableExtensionCount = 0;
		std::vector<VkExtensionProperties> availableExtensions;

		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &availableExtensionCount, nullptr);

		availableExtensions.resize(availableExtensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &availableExtensionCount, availableExtensions.data());

		for (const auto& rex : this->_swapchainRequiredExtensions) {
			bool found = false;
			for (const auto& aex : availableExtensions) {
				if (strcmp(rex, aex.extensionName) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
#if _DEBUG
				VkPhysicalDeviceProperties deviceProps;
				vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProps);
				LOGGER_ERROR("Swapchain Extension not available for device: {0} {1}", deviceProps.deviceName, rex);
#endif
				return false;
			}
		}
#if _DEBUG
		LOGGER_INFO("Swapchain Extension is supported!");
#endif
		return true;
	}

	SwapchainSupportDetails SwapchainManager::querySwapchainSupport(const VkPhysicalDevice& vkPhysicalDevice, const VkSurfaceKHR& vkSurfaceKHR) {
		if (!this->_swapchainSupportDetails.surfaceFormats.empty() && !this->_swapchainSupportDetails.surfacePresentModes.empty()) {
			return this->_swapchainSupportDetails;
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurfaceKHR, &this->_swapchainSupportDetails.surfaceCapabilites);
		
		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &surfaceFormatCount, nullptr);

		if (surfaceFormatCount > 0) {
			this->_swapchainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &surfaceFormatCount, this->_swapchainSupportDetails.surfaceFormats.data());
		}

		uint32_t surfacePresentModesCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurfaceKHR, &surfacePresentModesCount, nullptr);

		if (surfacePresentModesCount > 0) {
			this->_swapchainSupportDetails.surfacePresentModes.resize(surfacePresentModesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurfaceKHR, &surfacePresentModesCount, this->_swapchainSupportDetails.surfacePresentModes.data());
		}
		return this->_swapchainSupportDetails;
	}

	bool SwapchainManager::checkSwapchainDetailsSupport(const VkPhysicalDevice& vkPhysicalDevice, const VkSurfaceKHR& vkSurfaceKHR) {
		this->querySwapchainSupport(vkPhysicalDevice, vkSurfaceKHR);

#if _DEBUG
		if (!this->_swapchainSupportDetails.surfaceFormats.empty() && !this->_swapchainSupportDetails.surfacePresentModes.empty()) {
			LOGGER_INFO("Swapchain Details are supported");
		}
		else {
			LOGGER_ERROR("Swapchain Details are not supported");
		}
#endif // _DEBUG
		return !this->_swapchainSupportDetails.surfaceFormats.empty() && !this->_swapchainSupportDetails.surfacePresentModes.empty();
	}

	void SwapchainManager::createSwapChain(GLFWwindow* glfwWindow, const VkPhysicalDevice& vkPhysicalDevice,const VkDevice& vkLogicalDevice, const VkSurfaceKHR& vkSurfaceKHR, const std::shared_ptr<QueueManager>& queueManager) {
		this->querySwapchainSupport(vkPhysicalDevice, vkSurfaceKHR);

		this->_vkLogicalDevice = vkLogicalDevice;

		this->_swapchainConfiguration.vkSurfaceFormat = this->chooseSurfaceFormat();
		this->_swapchainConfiguration.vkPresentMode = this->choosePresentationMode();
		this->_swapchainConfiguration.vkExtent2D = this->chooseSwapExtent(glfwWindow);

		uint32_t imageCount = this->_swapchainSupportDetails.surfaceCapabilites.minImageCount + 1;

		if (this->_swapchainSupportDetails.surfaceCapabilites.maxImageCount > 0 && imageCount > this->_swapchainSupportDetails.surfaceCapabilites.maxImageCount) {
			imageCount = this->_swapchainSupportDetails.surfaceCapabilites.maxImageCount;
		}

		VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR{};
		vkSwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vkSwapchainCreateInfoKHR.surface = vkSurfaceKHR;
		vkSwapchainCreateInfoKHR.minImageCount = imageCount;
		vkSwapchainCreateInfoKHR.imageFormat = this->_swapchainConfiguration.vkSurfaceFormat.format;
		vkSwapchainCreateInfoKHR.imageColorSpace = this->_swapchainConfiguration.vkSurfaceFormat.colorSpace;
		vkSwapchainCreateInfoKHR.imageExtent = this->_swapchainConfiguration.vkExtent2D;
		vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
		vkSwapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { queueManager->getQueueFamilyIndices().graphisFamily.value(), queueManager->getQueueFamilyIndices().presentFamily.value() };

		if (queueManager->getQueueFamilyIndices().graphisFamily.value() != queueManager->getQueueFamilyIndices().presentFamily.value()) {
			vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vkSwapchainCreateInfoKHR.queueFamilyIndexCount = 2;
			vkSwapchainCreateInfoKHR.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vkSwapchainCreateInfoKHR.queueFamilyIndexCount = 0;
			vkSwapchainCreateInfoKHR.pQueueFamilyIndices = 0;
		}

		vkSwapchainCreateInfoKHR.preTransform = this->_swapchainSupportDetails.surfaceCapabilites.currentTransform;
		vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vkSwapchainCreateInfoKHR.presentMode = this->_swapchainConfiguration.vkPresentMode;
		vkSwapchainCreateInfoKHR.clipped = VK_TRUE;
		vkSwapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(vkLogicalDevice, &vkSwapchainCreateInfoKHR, nullptr, &this->_vkSwapChainKHR) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Swap Chain");
		}

		uint32_t swapchainImageCount = 0;
		vkGetSwapchainImagesKHR(vkLogicalDevice, this->_vkSwapChainKHR, &swapchainImageCount, nullptr);

		this->_swapChainImages.resize(swapchainImageCount);
		vkGetSwapchainImagesKHR(vkLogicalDevice, this->_vkSwapChainKHR, &swapchainImageCount, this->_swapChainImages.data());

#if _DEBUG || _RELEASE 
		LOGGER_INFO("Successfully created Swap Chain {}", swapchainImageCount);
#endif
	}

	void SwapchainManager::createImageViews(const VkDevice& vkLogicalDevice) {
		//this->_swapChainImageViews.resize(1);
		for (auto& sci : this->_swapChainImages) {
			VkImageViewCreateInfo vkImageViewCreateInfo{};
			vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vkImageViewCreateInfo.image = sci;
			vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			vkImageViewCreateInfo.format = this->_swapchainConfiguration.vkSurfaceFormat.format;
			vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			vkImageViewCreateInfo.subresourceRange.levelCount = 1;
			vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			vkImageViewCreateInfo.subresourceRange.layerCount = 1;
			
			VkImageView imageView;
			if (vkCreateImageView(vkLogicalDevice, &vkImageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Swap Chain Image View");
			}
			this->_swapChainImageViews.push_back(imageView);
		}
#if _DEBUG
		LOGGER_INFO("Successfully created Image Views {0} {1}", this->_swapChainImages.size(), this->_swapChainImageViews.size());
#endif
	}

	VkSurfaceFormatKHR SwapchainManager::chooseSurfaceFormat() {
		for (const auto& asf : this->_swapchainSupportDetails.surfaceFormats) {
			if (asf.format == VK_FORMAT_B8G8R8A8_SRGB && asf.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return asf;
			}
		}
		return this->_swapchainSupportDetails.surfaceFormats[0];
	}

	VkPresentModeKHR SwapchainManager::choosePresentationMode() {
		for (const auto& spm : this->_swapchainSupportDetails.surfacePresentModes) {
			if (spm == VK_PRESENT_MODE_MAILBOX_KHR) {
				return spm;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapchainManager::chooseSwapExtent(GLFWwindow* window) {
		if (this->_swapchainSupportDetails.surfaceCapabilites.currentExtent.width != UINT32_MAX) {
			return this->_swapchainSupportDetails.surfaceCapabilites.currentExtent;
		}
		int width; int height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width,
			this->_swapchainSupportDetails.surfaceCapabilites.minImageExtent.width,
			this->_swapchainSupportDetails.surfaceCapabilites.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height,
			this->_swapchainSupportDetails.surfaceCapabilites.minImageExtent.height,
			this->_swapchainSupportDetails.surfaceCapabilites.maxImageExtent.height
		);

		return actualExtent;
	}

	SwapchainManager::~SwapchainManager() {
		vkDestroySwapchainKHR(this->_vkLogicalDevice, this->_vkSwapChainKHR, nullptr);
		for (auto& siv : this->_swapChainImageViews) {
			vkDestroyImageView(this->_vkLogicalDevice, siv, nullptr);
		}
	}
}