#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_device.h"

namespace mz {
	class VulkanSwapChain {
	private:
		VkSurfaceFormatKHR m_surfaceFormat;
		VkPresentModeKHR m_presentMode;
		VkExtent2D m_extent;
		uint32_t m_imageCount;

		VkSwapchainKHR m_swapChain;
		
		std::vector<VkImage> m_images;

		VkSurfaceKHR m_surface;
		SwapChainSupportDetails& m_swapChainSupportDetails;
		QueueFamilyIndices& m_queueFamilyIndices;
		VkDevice m_device;

		void ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		void ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	public:
		void Create();
		void Destroy(VkAllocationCallbacks* allocator = nullptr);
		VulkanSwapChain(VkSurfaceKHR surface, VkDevice device, SwapChainSupportDetails& swapChainSupportDetails, QueueFamilyIndices& queueFamilyIndices);
	};
}