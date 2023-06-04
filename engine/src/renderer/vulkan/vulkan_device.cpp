#include "vulkan_device.h"
#include "engine/src/core/log.h"

namespace mz {
	bool VulkanDevice::SelectPhysicalDevice(VkInstance instance)
	{
		MZ_CORE_TRACE("Selecting physical device...");

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			MZ_CORE_CRITICAL("Failed to find GPUs with Vulkan device support!");
			return false;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			QueueFamilyIndices indices = FindQueueFamilies(device, m_surface);
			SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(device, m_surface);

			if (IsDeviceSuitable(device, indices, swapChainSupportDetails)) {
				m_physicalDevice = device;
				m_queueFamilyIndices = indices;
				m_swapChainSupportDetails = swapChainSupportDetails;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE) {
			return false;
		}

		MZ_CORE_INFO("Physical device selected!");
		return true;
	}

	bool VulkanDevice::CreateLogicalDevice(const std::vector<const char*> validationLayers, VkAllocationCallbacks* allocator) 
	{
		MZ_CORE_TRACE("Creating logical device...");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_requiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_requiredDeviceExtensions.data();

		if (vkCreateDevice(m_physicalDevice, &createInfo, allocator, &m_device) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);

		MZ_CORE_INFO("Created logical device!");
	}

	VulkanDevice::VulkanDevice(VkSurfaceKHR surface)
	{
		m_physicalDevice = VK_NULL_HANDLE;
		m_device = VK_NULL_HANDLE;
		m_presentQueue = VK_NULL_HANDLE;
		m_graphicsQueue = VK_NULL_HANDLE;
		m_surface = surface;
	}

	SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	void VulkanDevice::Shutdown(VkAllocationCallbacks* allocator) {
		MZ_CORE_TRACE("Destroying device...");
		vkDestroyDevice(m_device, allocator);
	}
	
	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices, SwapChainSupportDetails swapChainDetails) {
		if (!indices.isComplete()) {
			return false;
		}
		
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if (!deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
			!deviceFeatures.geometryShader) {
			return false;
		}

		if (!CheckDeviceExtensionSupport(device)) {
			return false;
		}

		if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) {
			return false;
		}

		return true;
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(s_requiredDeviceExtensions.begin(), s_requiredDeviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}