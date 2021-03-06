#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#pragma warning(pop)

#include <vector>

#include ".\QueueManager.h"
#include "..\Logger.h"

namespace va {

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilites;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> surfacePresentModes;
	};

	struct SwapchainConfiguration
	{
		VkExtent2D vkExtent2D;
		VkSurfaceFormatKHR vkSurfaceFormat;
		VkPresentModeKHR vkPresentMode;
	};

	class SwapchainManager
	{
	private:
		std::vector<const char*> _swapchainRequiredExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		SwapchainSupportDetails _swapchainSupportDetails{};
		SwapchainConfiguration _swapchainConfiguration{};

		VkSwapchainKHR _vkSwapChainKHR;
		VkDevice _vkLogicalDevice;

		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;

		VkSurfaceFormatKHR chooseSurfaceFormat();
		VkPresentModeKHR choosePresentationMode();
		VkExtent2D chooseSwapExtent(GLFWwindow*);
		SwapchainSupportDetails querySwapchainSupport(const VkPhysicalDevice&, const VkSurfaceKHR&);

	public:
		SwapchainManager();
		~SwapchainManager();

		bool checkSwapchainExtensionsSupport(const VkPhysicalDevice&);
		bool checkSwapchainDetailsSupport(const VkPhysicalDevice&, const VkSurfaceKHR&);

		void createSwapChain(GLFWwindow*, const VkPhysicalDevice& , const VkDevice&, const VkSurfaceKHR&, const std::shared_ptr<QueueManager>&);
		void createImageViews(const VkDevice&);
		void cleanSwapChain();

		inline const std::vector<const char*>& requiredDeviceExtensions() { return this->_swapchainRequiredExtensions; }
		inline const VkExtent2D& getExtent() { return this->_swapchainConfiguration.vkExtent2D; }
		inline const VkSurfaceFormatKHR& getSwapchainSurfaceFormat() { return this->_swapchainConfiguration.vkSurfaceFormat; }
		inline const std::vector<VkImage>& getSwapchainImages() const { return this->_swapChainImages; }
		inline const std::vector<VkImageView>& getSwapchainImageViews() const { return this->_swapChainImageViews; }
		inline const VkSwapchainKHR& getSwapchain() const { return this->_vkSwapChainKHR; }

		// Deactivating Copy Constructor and Overlaoded Assignment Operator
		SwapchainManager(const SwapchainManager&) = delete;
		const SwapchainManager& operator=(const SwapchainManager&) = delete;
	};
}



