#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanDevice {
	private:
		std::shared_ptr<VulkanContext> m_contextPtr;

		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;

		QueueFamilyIndices m_queueFamilyIndices;

		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		SwapChainSupportDetails m_swapChainSupportDetails;

		VkSurfaceKHR m_surface;

		VkCommandPool m_graphicsCommandPool;

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

		inline SwapChainSupportDetails& GetSwapChainSupportDetails() { return m_swapChainSupportDetails; }
		inline QueueFamilyIndices& GetQueueFamilyIndices() { return m_queueFamilyIndices; }
		inline VkDevice GetLogicalDevice() { return m_device; }
		bool CreateGraphicsCommandPool();
		void DestroyGraphicsCommandPool();
		inline VkCommandPool GetGraphicsCommandPool() { return m_graphicsCommandPool; }
		inline VkQueue GetGraphicsQueue() { return m_graphicsQueue; }
		inline VkQueue GetPresentQueue() { return m_presentQueue; }
	};

	const std::vector<const char*> VulkanDevice::s_requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}