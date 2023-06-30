#include "vulkan_device.h"
#include "engine/src/core/log.h"

namespace mz {
	bool VulkanDevice::SelectPhysicalDevice()
	{
		MZ_CORE_TRACE("Selecting physical device...");

		auto instance = s_contextPtr->instance;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			MZ_CORE_CRITICAL("Failed to find GPUs with Vulkan device support!");
			return false;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			QueueFamilyIndices indices = FindQueueFamilies(device, s_contextPtr->surface);
			SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(device, s_contextPtr->surface);

			if (IsDeviceSuitable(device, indices, swapChainSupportDetails)) {
				s_contextPtr->device.physicalDevice = device;
				s_contextPtr->device.queueFamalies = indices;
				s_contextPtr->device.swapChainDetails = swapChainSupportDetails;
				break;
			}
		}

		if (s_contextPtr->device.physicalDevice == VK_NULL_HANDLE) {
			return false;
		}

		MZ_CORE_INFO("Physical device selected!");
		return true;
	}

	bool VulkanDevice::CreateLogicalDevice() 
	{
		MZ_CORE_TRACE("Creating logical device...");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { s_contextPtr->device.queueFamalies.graphicsFamily.value(), s_contextPtr->device.queueFamalies.presentFamily.value() };

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
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledLayerCount = static_cast<uint32_t>(s_contextPtr->validationLayers.size());
		createInfo.ppEnabledLayerNames = s_contextPtr->validationLayers.data();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_requiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_requiredDeviceExtensions.data();

		if (vkCreateDevice(s_contextPtr->device.physicalDevice, &createInfo, s_contextPtr->allocator, &s_contextPtr->device.logicalDevice) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(s_contextPtr->device.logicalDevice, s_contextPtr->device.queueFamalies.graphicsFamily.value(), 0, &s_contextPtr->device.graphicsQueue);
		vkGetDeviceQueue(s_contextPtr->device.logicalDevice, s_contextPtr->device.queueFamalies.presentFamily.value(), 0, &s_contextPtr->device.presentQueue);

		MZ_CORE_INFO("Created logical device!");
		return true;
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

	void VulkanDevice::Shutdown() {
		MZ_CORE_TRACE("Destroying device...");
		vkDestroyDevice(s_contextPtr->device.logicalDevice, s_contextPtr->allocator);
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
		
		vkGetPhysicalDeviceProperties(device, &s_contextPtr->device.physicalDeviceProperties);
		
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if (!s_contextPtr->device.physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
			!deviceFeatures.geometryShader) {
			return false;
		}

		if (!CheckDeviceExtensionSupport(device)) {
			return false;
		}

		if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty()) {
			return false;
		}

		if (!deviceFeatures.samplerAnisotropy) {
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

	bool VulkanDevice::FindDepthFormat()
	{
		if (FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
			s_contextPtr->device.depthFormat)) 
		{
			MZ_CORE_TRACE("Found device depth format!");
			return true;
		}

		MZ_CORE_CRITICAL("Failed to find depth format for device!");

		return false;
	}
	
	bool VulkanDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat& outFormat)
	{
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(s_contextPtr->device.physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				outFormat = format;
				return true;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				outFormat = format;
				return true;
			}
		}

		return false;
	}

	bool VulkanDevice::CreateGraphicsCommandPool() {
		MZ_CORE_TRACE("Creating graphics command pool...");
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = s_contextPtr->device.queueFamalies.graphicsFamily.value();

		if (vkCreateCommandPool(s_contextPtr->device.logicalDevice, &poolInfo, s_contextPtr->allocator, &s_contextPtr->device.graphicsCommandPool) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create graphics command pool!");
			return false;
		}

		return true;
	}

	void VulkanDevice::DestroyGraphicsCommandPool() {
		MZ_CORE_TRACE("Destroying graphics command pool...");
		vkDestroyCommandPool(s_contextPtr->device.logicalDevice, s_contextPtr->device.graphicsCommandPool, s_contextPtr->allocator);
	}
}