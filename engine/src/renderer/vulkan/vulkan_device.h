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

		static bool IsDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies();
	public:
		bool SelectPhysicalDevice(VkInstance& instance);
		VulkanDevice();
	};
}