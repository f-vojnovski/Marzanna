#include "vulkan_swap_chain.h"
#include "vulkan_utils.h"
#include "vulkan_device.h"

namespace mz {
	void VulkanSwapChain::Create()
	{
		MZ_CORE_TRACE("Creating swap chain...");

		s_contextPtr->device.swapChainDetails = VulkanDevice::QuerySwapChainSupport(s_contextPtr->device.physicalDevice, s_contextPtr->surface);

		ChooseSurfaceFormat();
		ChoosePresentMode();
		ChooseExtent();

		s_contextPtr->swapChain.imageCount = s_contextPtr->device.swapChainDetails.capabilities.minImageCount + 1;
		if (s_contextPtr->device.swapChainDetails.capabilities.maxImageCount > 0 && s_contextPtr->swapChain.imageCount > s_contextPtr->device.swapChainDetails.capabilities.maxImageCount) {
			s_contextPtr->swapChain.imageCount = s_contextPtr->device.swapChainDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = s_contextPtr->surface;

		createInfo.minImageCount = s_contextPtr->swapChain.imageCount;
		createInfo.imageFormat = s_contextPtr->swapChain.surfaceFormat.format;
		createInfo.imageColorSpace = s_contextPtr->swapChain.surfaceFormat.colorSpace;
		createInfo.imageExtent = s_contextPtr->swapChain.extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { s_contextPtr->device.queueFamalies.graphicsFamily.value(), s_contextPtr->device.queueFamalies.presentFamily.value() };

		if (s_contextPtr->device.queueFamalies.graphicsFamily != s_contextPtr->device.queueFamalies.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = s_contextPtr->device.swapChainDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = s_contextPtr->swapChain.presentMode;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(s_contextPtr->device.logicalDevice, &createInfo, nullptr, &s_contextPtr->swapChain.handle) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create swap chain!");
			return;
		}

		vkGetSwapchainImagesKHR(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.handle, &s_contextPtr->swapChain.imageCount, nullptr);
		s_contextPtr->swapChain.images.resize(s_contextPtr->swapChain.imageCount);
		vkGetSwapchainImagesKHR(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.handle, &s_contextPtr->swapChain.imageCount, s_contextPtr->swapChain.images.data());

		CreateImageViews();

		MZ_CORE_INFO("Swap chain created!");
	}
	
	void VulkanSwapChain::Destroy()
	{
		for (auto imageView : s_contextPtr->swapChain.imageViews) {
			vkDestroyImageView(s_contextPtr->device.logicalDevice, imageView, s_contextPtr->allocator);
		}

		vkDestroySwapchainKHR(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.handle, s_contextPtr->allocator);
	}

	void VulkanSwapChain::CreateImageViews()
	{
		s_contextPtr->swapChain.imageViews.resize(s_contextPtr->swapChain.images.size());

		for (size_t i = 0; i < s_contextPtr->swapChain.images.size(); i++) {
			s_contextPtr->swapChain.imageViews[i] = VulkanFunctions::CreateImageView(s_contextPtr->swapChain.images[i], s_contextPtr->swapChain.surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	bool VulkanSwapChain::CreateSyncObjects()
	{
		MZ_CORE_TRACE("Creating swap chain sync objects...");

		s_contextPtr->swapChain.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		s_contextPtr->swapChain.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		s_contextPtr->swapChain.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(s_contextPtr->device.logicalDevice, &semaphoreInfo, nullptr, &s_contextPtr->swapChain.imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(s_contextPtr->device.logicalDevice, &semaphoreInfo, nullptr, &s_contextPtr->swapChain.renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(s_contextPtr->device.logicalDevice, &fenceInfo, nullptr, &s_contextPtr->swapChain.inFlightFences[i]) != VK_SUCCESS) {

				return false;
			}
		}

		MZ_CORE_INFO("Created swap chain sync objects!");

		return true;
	}

	void VulkanSwapChain::DestroySyncObjects()
	{
		MZ_CORE_TRACE("Destroying sync objects...");

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.imageAvailableSemaphores[i], s_contextPtr->allocator);
			vkDestroySemaphore(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.renderFinishedSemaphores[i], s_contextPtr->allocator);
			vkDestroyFence(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.inFlightFences[i], s_contextPtr->allocator);
		}
	}

	bool VulkanSwapChain::CreateFramebuffers()
    {
        s_contextPtr->swapChain.framebuffers.resize(s_contextPtr->swapChain.imageViews.size());

        for (size_t i = 0; i < s_contextPtr->swapChain.imageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                s_contextPtr->swapChain.imageViews[i],
				s_contextPtr->swapChain.depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = s_contextPtr->mainRenderPass.handle;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = s_contextPtr->swapChain.extent.width;
            framebufferInfo.height = s_contextPtr->swapChain.extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(s_contextPtr->device.logicalDevice, &framebufferInfo, s_contextPtr->allocator, &s_contextPtr->swapChain.framebuffers[i]) != VK_SUCCESS) {
                MZ_CORE_ERROR("Failed to create framebuffer!");
                return false;
            }
        }
    }

    void VulkanSwapChain::DestroyFramebuffers()
    {
        MZ_CORE_TRACE("Destroying framebuffers...");
        for (auto framebuffer : s_contextPtr->swapChain.framebuffers) {
            vkDestroyFramebuffer(s_contextPtr->device.logicalDevice, framebuffer, s_contextPtr->allocator);
        }
    }

	VkResult VulkanSwapChain::AcquireNextImageIndex()
	{
		VkResult result = vkAcquireNextImageKHR(
			s_contextPtr->device.logicalDevice,
			s_contextPtr->swapChain.handle,
			UINT64_MAX,
			s_contextPtr->swapChain.imageAvailableSemaphores[s_contextPtr->currentFrame],
			VK_NULL_HANDLE,
			&s_contextPtr->swapChain.nextImageIndex);

		return result;
	}

	void VulkanSwapChain::Cleanup()
	{
		DestroyFramebuffers();

		for (size_t i = 0; i < s_contextPtr->swapChain.imageViews.size(); i++) {
			vkDestroyImageView(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.imageViews[i], s_contextPtr->allocator);
		}

		vkDestroySwapchainKHR(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.handle, s_contextPtr->allocator);
	}

	bool VulkanSwapChain::Recreate()
	{
		s_contextPtr->swapChain.recreating = false;
		vkDeviceWaitIdle(s_contextPtr->device.logicalDevice);
		Cleanup();
		Create();
		return CreateFramebuffers();
	}

	bool VulkanSwapChain::CreateDepthResources()
	{
		if (!VulkanDevice::FindDepthFormat()) {
			return false;
		}

		if (!VulkanFunctions::CreateImage(
			s_contextPtr->swapChain.extent.width, s_contextPtr->swapChain.extent.height,
			s_contextPtr->device.depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			s_contextPtr->swapChain.depthImage,
			s_contextPtr->swapChain.depthImageMemory)) {
			MZ_CORE_CRITICAL("Failed to create depth image!");
			return false;
		}

		s_contextPtr->swapChain.depthImageView = VulkanFunctions::CreateImageView(s_contextPtr->swapChain.depthImage, s_contextPtr->device.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		if (s_contextPtr->swapChain.depthImageView == VK_NULL_HANDLE) {
			MZ_CORE_CRITICAL("Failed to create depth image view!");
			return false;
		}

		MZ_CORE_INFO("Created depth resources!");
		return true;
	}

	
	void VulkanSwapChain::DestroyDepthResources()
	{
		MZ_CORE_TRACE("Destroying depth resources...");
		vkDestroyImageView(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.depthImageView, s_contextPtr->allocator);
		vkDestroyImage(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.depthImage, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, s_contextPtr->swapChain.depthImageMemory, s_contextPtr->allocator);
	}

	void VulkanSwapChain::ChooseSurfaceFormat()
	{
		for (const auto& availableFormat : s_contextPtr->device.swapChainDetails.formats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				s_contextPtr->swapChain.surfaceFormat = availableFormat;
				return;
			}
		}
		s_contextPtr->swapChain.surfaceFormat = s_contextPtr->device.swapChainDetails.formats[0];
	}

	void VulkanSwapChain::ChoosePresentMode()
	{
		for (const auto& availablePresentMode : s_contextPtr->device.swapChainDetails.presentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				s_contextPtr->swapChain.presentMode = availablePresentMode;
				return;
			}
		}

		s_contextPtr->swapChain.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
	
	void VulkanSwapChain::ChooseExtent()
	{
		if (s_contextPtr->device.swapChainDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			s_contextPtr->swapChain.extent = s_contextPtr->device.swapChainDetails.capabilities.currentExtent;
		}
		else {
			unsigned int width = Application::Get().GetWindow().GetFramebufferWidth();
			unsigned int height = Application::Get().GetWindow().GetFramebufferHeight();

			s_contextPtr->swapChain.extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			s_contextPtr->swapChain.extent.width = 
				std::clamp(
					s_contextPtr->swapChain.extent.width, 
					s_contextPtr->device.swapChainDetails.capabilities.minImageExtent.width, 
					s_contextPtr->device.swapChainDetails.capabilities.maxImageExtent.width);
			s_contextPtr->swapChain.extent.height =
				std::clamp(
					s_contextPtr->swapChain.extent.height,
					s_contextPtr->device.swapChainDetails.capabilities.minImageExtent.height,
					s_contextPtr->device.swapChainDetails.capabilities.maxImageExtent.height);
		}
	}
}