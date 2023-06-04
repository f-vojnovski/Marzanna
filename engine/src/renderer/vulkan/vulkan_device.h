#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	class VulkanDevice {
	private:
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;

		QueueFamilyIndices m_queueFamilyIndices;

		static bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices);
		static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	public:
		bool SelectPhysicalDevice(VkInstance& instance);
		bool CreateLogicalDevice(const std::vector<const char*> validationLayers, VkAllocationCallbacks* allocator);
		void Shutdown(VkAllocationCallbacks* allocator);
		VulkanDevice();
	};
}