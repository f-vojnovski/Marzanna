#include "vulkan_swap_chain.h"
#include "vulkan_utils.h"

namespace mz {
	VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, VkDevice device, SwapChainSupportDetails& swapChainSupportDetails, QueueFamilyIndices& queueFamilyIndices)
		: m_swapChainSupportDetails(swapChainSupportDetails), m_queueFamilyIndices(queueFamilyIndices)
	{
		m_surface = surface;
		m_device = device;
	}

	void VulkanSwapChain::Create()
	{
		MZ_CORE_TRACE("Creating swap chain...");

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

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create swap chain!");
			return;
		}

		vkGetSwapchainImagesKHR(m_device, m_swapChain, &m_imageCount, nullptr);
		m_images.resize(m_imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapChain, &m_imageCount, m_images.data());

		CreateImageViews();

		MZ_CORE_INFO("Swap chain created!");
	}
	
	void VulkanSwapChain::Destroy(VkAllocationCallbacks* allocator)
	{
		for (auto imageView : m_imageViews) {
			vkDestroyImageView(m_device, imageView, allocator);
		}

		vkDestroySwapchainKHR(m_device, m_swapChain, allocator);
	}

	void VulkanSwapChain::CreateImageViews()
	{
		m_imageViews.resize(m_images.size());

		for (size_t i = 0; i < m_images.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_images[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_surfaceFormat.format;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(m_device, &createInfo, nullptr, &m_imageViews[i]));
		}
	}

	bool VulkanSwapChain::CreateFramebuffers(VkRenderPass renderPass)
	{
		m_framebuffers.resize(m_imageViews.size());

		for (size_t i = 0; i < m_imageViews.size(); i++) {
			VkImageView attachments[] = {
				m_imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_extent.width;
			framebufferInfo.height = m_extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
				MZ_CORE_ERROR("Failed to create framebuffer!");
				return false;
			}
		}
	}

	
	void VulkanSwapChain::DestroyFramebuffers()
	{
		MZ_CORE_TRACE("Destroying framebuffers...");
		for(auto framebuffer : m_framebuffers) {
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}
	}

	bool VulkanSwapChain::CreateSyncObjects()
	{
		MZ_CORE_TRACE("Creating swap chain sync objects...");
		
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS) {
			return false;
		}
		
		if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS) {
			return false;
		}

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS) {
			return false;
		}


		MZ_CORE_INFO("Created swap chain sync objects!");

		return true;
	}

	void VulkanSwapChain::DestroySyncObjects()
	{
		MZ_CORE_TRACE("Destroying sync objects...");

		vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
		vkDestroyFence(m_device, m_inFlightFence, nullptr);
	}

	bool VulkanSwapChain::AcquireNextImageIndex()
	{
		VkResult result = vkAcquireNextImageKHR(
			m_device,
			m_swapChain,
			UINT64_MAX,
			m_imageAvailableSemaphore,
			VK_NULL_HANDLE,
			&m_nextImageIndex);

		if (result != VK_SUCCESS) {
			return false;
		}
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