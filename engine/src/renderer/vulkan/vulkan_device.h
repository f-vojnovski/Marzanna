#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanDevice {
	public:
		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		static const std::vector<const char*> s_requiredDeviceExtensions;

		bool SelectPhysicalDevice();
		bool CreateLogicalDevice();
		void Shutdown();
		bool CreateGraphicsCommandPool();
		void DestroyGraphicsCommandPool();
		static bool FindDepthFormat();
		static bool FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat& outFormat);

		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		static bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices, SwapChainSupportDetails swapChainDetails);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	};

	const std::vector<const char*> VulkanDevice::s_requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}