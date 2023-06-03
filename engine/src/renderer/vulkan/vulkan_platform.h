#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/window.h"

namespace mz {
	void PlatformGetRequiredExtensionNames(std::vector<const char*>& extensionNames);

	// TODO: Send context instead of this 
	bool PlatformCreateVulkanSurface(VkInstance instance, Window* window, VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface);
}