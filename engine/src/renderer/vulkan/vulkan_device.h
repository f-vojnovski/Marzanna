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

	class VulkanDevice {
	private:
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;

		QueueFamilyIndices m_queueFamilyIndices;

		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		static bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices);
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	public:
		bool SelectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
		bool CreateLogicalDevice(const std::vector<const char*> validationLayers, VkAllocationCallbacks* allocator);
		void Shutdown(VkAllocationCallbacks* allocator);
		VulkanDevice();
	};
}