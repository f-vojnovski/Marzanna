#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	// Device
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

	struct VulkanDeviceInfo {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		QueueFamilyIndices queueFamalies;
		SwapChainSupportDetails swapChainDetails;

		VkSurfaceKHR surface;

		VkCommandPool graphicsCommandPool;
	};
	
	struct VulkanContext {
		VulkanDeviceInfo device;
	};
}