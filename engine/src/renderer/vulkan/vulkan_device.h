#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanDevice {
	private:
		std::shared_ptr<VulkanContext> contextPtr;

		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		static bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices, SwapChainSupportDetails swapChainDetails);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	public:
		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		static const std::vector<const char*> s_requiredDeviceExtensions;

		VulkanDevice(std::shared_ptr<VulkanContext> contextPtr);
		bool SelectPhysicalDevice();
		bool CreateLogicalDevice();
		void Shutdown();
		bool CreateGraphicsCommandPool();
		void DestroyGraphicsCommandPool();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};

	const std::vector<const char*> VulkanDevice::s_requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}