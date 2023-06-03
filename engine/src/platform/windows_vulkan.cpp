#include "windows_vulkan.h"
#include "windows_window.h"
#include "engine/src/core/log.h"
#include "engine/src/mzpch.h"

namespace mz {
	void PlatformGetRequiredExtensionNames(std::vector<const char*>& extensionNames) {
		extensionNames.push_back("VK_KHR_win32_surface");
	}
	bool PlatformCreateVulkanSurface(VkInstance instance, Window* window, VkAllocationCallbacks* allocator, VkSurfaceKHR* outSurface)
	{
		auto glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
		VkResult result = glfwCreateWindowSurface(instance, glfwWindow, allocator, outSurface);

		if (result != VK_SUCCESS) {
			MZ_CORE_CRITICAL("Failed to create Vulkan surface!");
			return false;
		}

		return true;
	}
}