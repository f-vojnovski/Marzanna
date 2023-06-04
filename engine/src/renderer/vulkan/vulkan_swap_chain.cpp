#include "vulkan_swap_chain.h"

namespace mz {
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, VkDevice device, SwapChainSupportDetails& swapChainSupportDetails, QueueFamilyIndices& queueFamilyIndices)
		: m_swapChainSupportDetails(swapChainSupportDetails), m_queueFamilyIndices(queueFamilyIndices)
	{
		m_surface = surface;
		m_device = device;
	}

	void VulkanSwapChain::Create()
	{
		ChooseSurfaceFormat(m_swapChainSupportDetails.formats);
		ChoosePresentMode(m_swapChainSupportDetails.presentModes);
		ChooseExtent(m_swapChainSupportDetails.capabilities);

		m_imageCount = m_swapChainSupportDetails.capabilities.minImageCount + 1;
		if (m_swapChainSupportDetails.capabilities.maxImageCount > 0 && m_imageCount > m_swapChainSupportDetails.capabilities.maxImageCount) {
			m_imageCount = m_swapChainSupportDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;

		createInfo.minImageCount = m_imageCount;
		createInfo.imageFormat = m_surfaceFormat.format;
		createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
		createInfo.imageExtent = m_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value() };

		if (m_queueFamilyIndices.graphicsFamily != m_queueFamilyIndices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = m_swapChainSupportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = m_presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create swap chain!");
		}
	}
	
	void VulkanSwapChain::Destroy(VkAllocationCallbacks* allocator)
	{
		vkDestroySwapchainKHR(m_device, swapChain, allocator);
	}

	void VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_surfaceFormat = availableFormat;
				return;
			}
		}
		m_surfaceFormat = availableFormats[0];
	}

	void VulkanSwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				m_presentMode = availablePresentMode;
				return;
			}
		}

		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
	
	void VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			m_extent = capabilities.currentExtent;
		}
		else {
			unsigned int height = Application::Get().GetWindow().GetHeight();
			unsigned int width = Application::Get().GetWindow().GetWidth();

			m_extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			m_extent.width = std::clamp(m_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			m_extent.height = std::clamp(m_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		}
	}
}