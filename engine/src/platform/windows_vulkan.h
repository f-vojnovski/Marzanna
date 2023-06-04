#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include "engine/src/mzpch.h"
#include "engine/src/core/window.h"

namespace mz {
	void PlatformGetRequiredExtensionNames(std::vector<const char*>& extensionNames);

	bool PlatformCreateVulkanSurface(VkInstance instance, const Window* window, VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface);
}

#endif