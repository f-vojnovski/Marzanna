#include "vulkan_device.h"
#include "engine/src/core/log.h"

namespace mz {
	VulkanDevice::VulkanDevice(std::shared_ptr<VulkanContext> contextPtr)
	{
		this->contextPtr = contextPtr;
	}

	bool VulkanDevice::SelectPhysicalDevice()
	{
		MZ_CORE_TRACE("Selecting physical device...");

		auto instance = contextPtr->instance;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			MZ_CORE_CRITICAL("Failed to find GPUs with Vulkan device support!");
			return false;
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			QueueFamilyIndices indices = FindQueueFamilies(device, contextPtr->surface);
			SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(device, contextPtr->surface);

			if (IsDeviceSuitable(device, indices, swapChainSupportDetails)) {
				contextPtr->device.physicalDevice = device;
				contextPtr->device.queueFamalies = indices;
				contextPtr->device.swapChainDetails = swapChainSupportDetails;
				break;
			}
		}

		if (contextPtr->device.physicalDevice == VK_NULL_HANDLE) {
			return false;
		}

		MZ_CORE_INFO("Physical device selected!");
		return true;
	}

	bool VulkanDevice::CreateLogicalDevice() 
	{
		MZ_CORE_TRACE("Creating logical device...");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { contextPtr->device.queueFamalies.graphicsFamily.value(), contextPtr->device.queueFamalies.presentFamily.value() };

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

		createInfo.enabledLayerCount = static_cast<uint32_t>(contextPtr->validationLayers.size());
		createInfo.ppEnabledLayerNames = contextPtr->validationLayers.data();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_requiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_requiredDeviceExtensions.data();

		if (vkCreateDevice(contextPtr->device.physicalDevice, &createInfo, contextPtr->allocator, &contextPtr->device.logicalDevice) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(contextPtr->device.logicalDevice, contextPtr->device.queueFamalies.graphicsFamily.value(), 0, &contextPtr->device.graphicsQueue);
		vkGetDeviceQueue(contextPtr->device.logicalDevice, contextPtr->device.queueFamalies.presentFamily.value(), 0, &contextPtr->device.presentQueue);

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
		vkDestroyDevice(contextPtr->device.logicalDevice, contextPtr->allocator);
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

	bool VulkanDevice::CreateGraphicsCommandPool() {
		MZ_CORE_TRACE("Creating graphics command pool...");
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = contextPtr->device.queueFamalies.graphicsFamily.value();

		if (vkCreateCommandPool(contextPtr->device.logicalDevice, &poolInfo, nullptr, &contextPtr->device.graphicsCommandPool) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create graphics command pool!");
			return false;
		}

		return true;
	}

	void VulkanDevice::DestroyGraphicsCommandPool() {
		MZ_CORE_TRACE("Destroying graphics command pool...");
		vkDestroyCommandPool(contextPtr->device.logicalDevice, contextPtr->device.graphicsCommandPool, nullptr);
	}
	
	uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(contextPtr->device.physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		return -1;
	}
}