#include "vulkan_swap_chain.h"
#include "vulkan_utils.h"

namespace mz {
	VulkanSwapChain::VulkanSwapChain(std::shared_ptr<VulkanContext> contextPtr) {
		this->contextPtr = contextPtr;
	}

	void VulkanSwapChain::Create()
	{
		MZ_CORE_TRACE("Creating swap chain...");

		ChooseSurfaceFormat();
		ChoosePresentMode();
		ChooseExtent();

		contextPtr->swapChain.imageCount = contextPtr->device.swapChainDetails.capabilities.minImageCount + 1;
		if (contextPtr->device.swapChainDetails.capabilities.maxImageCount > 0 && contextPtr->swapChain.imageCount > contextPtr->device.swapChainDetails.capabilities.maxImageCount) {
			contextPtr->swapChain.imageCount = contextPtr->device.swapChainDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = contextPtr->surface;

		createInfo.minImageCount = contextPtr->swapChain.imageCount;
		createInfo.imageFormat = contextPtr->swapChain.surfaceFormat.format;
		createInfo.imageColorSpace = contextPtr->swapChain.surfaceFormat.colorSpace;
		createInfo.imageExtent = contextPtr->swapChain.extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { contextPtr->device.queueFamalies.graphicsFamily.value(), contextPtr->device.queueFamalies.presentFamily.value() };

		if (contextPtr->device.queueFamalies.graphicsFamily != contextPtr->device.queueFamalies.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = contextPtr->device.swapChainDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = contextPtr->swapChain.presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(contextPtr->device.logicalDevice, &createInfo, nullptr, &contextPtr->swapChain.handle) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create swap chain!");
			return;
		}

		vkGetSwapchainImagesKHR(contextPtr->device.logicalDevice, contextPtr->swapChain.handle, &contextPtr->swapChain.imageCount, nullptr);
		contextPtr->swapChain.images.resize(contextPtr->swapChain.imageCount);
		vkGetSwapchainImagesKHR(contextPtr->device.logicalDevice, contextPtr->swapChain.handle, &contextPtr->swapChain.imageCount, contextPtr->swapChain.images.data());

		CreateImageViews();

		MZ_CORE_INFO("Swap chain created!");
	}
	
	void VulkanSwapChain::Destroy()
	{
		for (auto imageView : contextPtr->swapChain.imageViews) {
			vkDestroyImageView(contextPtr->device.logicalDevice, imageView, contextPtr->allocator);
		}

		vkDestroySwapchainKHR(contextPtr->device.logicalDevice, contextPtr->swapChain.handle, contextPtr->allocator);
	}

	void VulkanSwapChain::CreateImageViews()
	{
		contextPtr->swapChain.imageViews.resize(contextPtr->swapChain.images.size());

		for (size_t i = 0; i < contextPtr->swapChain.images.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = contextPtr->swapChain.images[i];

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = contextPtr->swapChain.surfaceFormat.format;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(contextPtr->device.logicalDevice, &createInfo, contextPtr->allocator, &contextPtr->swapChain.imageViews[i]));
		}
	}

	bool VulkanSwapChain::CreateSyncObjects()
	{
		MZ_CORE_TRACE("Creating swap chain sync objects...");
		
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		if (vkCreateSemaphore(contextPtr->device.logicalDevice, &semaphoreInfo, contextPtr->allocator, &contextPtr->swapChain.imageAvailableSemaphore) != VK_SUCCESS) {
			return false;
		}
		
		if (vkCreateSemaphore(contextPtr->device.logicalDevice, &semaphoreInfo, contextPtr->allocator, &contextPtr->swapChain.renderFinishedSemaphore) != VK_SUCCESS) {
			return false;
		}

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(contextPtr->device.logicalDevice, &fenceInfo, contextPtr->allocator, &contextPtr->swapChain.inFlightFence) != VK_SUCCESS) {
			return false;
		}

		MZ_CORE_INFO("Created swap chain sync objects!");

		return true;
	}

	void VulkanSwapChain::DestroySyncObjects()
	{
		MZ_CORE_TRACE("Destroying sync objects...");

		vkDestroySemaphore(contextPtr->device.logicalDevice, contextPtr->swapChain.imageAvailableSemaphore, contextPtr->allocator);
		vkDestroySemaphore(contextPtr->device.logicalDevice, contextPtr->swapChain.renderFinishedSemaphore, contextPtr->allocator);
		vkDestroyFence(contextPtr->device.logicalDevice, contextPtr->swapChain.inFlightFence, contextPtr->allocator);
	}

	bool VulkanSwapChain::AcquireNextImageIndex()
	{
		VkResult result = vkAcquireNextImageKHR(
			contextPtr->device.logicalDevice,
			contextPtr->swapChain.handle,
			UINT64_MAX,
			contextPtr->swapChain.imageAvailableSemaphore,
			VK_NULL_HANDLE,
			&contextPtr->swapChain.nextImageIndex);

		if (result != VK_SUCCESS) {
			return false;
		}
	}

	void VulkanSwapChain::ChooseSurfaceFormat()
	{
		for (const auto& availableFormat : contextPtr->device.swapChainDetails.formats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				contextPtr->swapChain.surfaceFormat = availableFormat;
				return;
			}
		}
		contextPtr->swapChain.surfaceFormat = contextPtr->device.swapChainDetails.formats[0];
	}

	void VulkanSwapChain::ChoosePresentMode()
	{
		for (const auto& availablePresentMode : contextPtr->device.swapChainDetails.presentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				contextPtr->swapChain.presentMode = availablePresentMode;
				return;
			}
		}

		contextPtr->swapChain.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
	
	void VulkanSwapChain::ChooseExtent()
	{
		if (contextPtr->device.swapChainDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			contextPtr->swapChain.extent = contextPtr->device.swapChainDetails.capabilities.currentExtent;
		}
		else {
			unsigned int height = Application::Get().GetWindow().GetHeight();
			unsigned int width = Application::Get().GetWindow().GetWidth();

			contextPtr->swapChain.extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			contextPtr->swapChain.extent.width = 
				std::clamp(
					contextPtr->swapChain.extent.width, 
					contextPtr->device.swapChainDetails.capabilities.minImageExtent.width, 
					contextPtr->device.swapChainDetails.capabilities.maxImageExtent.width);
			contextPtr->swapChain.extent.height =
				std::clamp(
					contextPtr->swapChain.extent.height,
					contextPtr->device.swapChainDetails.capabilities.minImageExtent.height,
					contextPtr->device.swapChainDetails.capabilities.maxImageExtent.height);
		}
	}
}