#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanDevice {
	private:
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;

		QueueFamilyIndices m_queueFamilyIndices;

		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		SwapChainSupportDetails m_swapChainSupportDetails;

		VkSurfaceKHR m_surface;

		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		static bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices, SwapChainSupportDetails swapChainDetails);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	public:
		static const std::vector<const char*> s_requiredDeviceExtensions;
		
		bool SelectPhysicalDevice(VkInstance instance);
		bool CreateLogicalDevice(const std::vector<const char*> validationLayers, VkAllocationCallbacks* allocator);
		void Shutdown(VkAllocationCallbacks* allocator);
		VulkanDevice(VkSurfaceKHR surface);
	};

	const std::vector<const char*> VulkanDevice::s_requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}