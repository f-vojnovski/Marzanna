#include "vulkan_device.h"
#include "engine/src/core/log.h"

namespace mz {
	bool VulkanDevice::SelectPhysicalDevice(VkInstance& instance)
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

		// TODO: Use score system
		for (const auto& device : devices) {
			QueueFamilyIndices indices = FindQueueFamilies(device);

			if (IsDeviceSuitable(device, indices)) {
				m_physicalDevice = device;
				m_queueFamilyIndices = indices;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE) {
			MZ_CORE_CRITICAL("Failed to find suitable physical device!");
			return false;
		}

		MZ_CORE_INFO("Physical device selected!");
		return true;
	}

	bool VulkanDevice::CreateLogicalDevice(const std::vector<const char*> validationLayers, VkAllocationCallbacks* allocator) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		if (vkCreateDevice(m_physicalDevice, &createInfo, allocator, &m_device) != VK_SUCCESS) {
			return false;
		}
	}

	VulkanDevice::VulkanDevice()
	{
		m_physicalDevice = VK_NULL_HANDLE;
		m_device = VK_NULL_HANDLE;
	}

	void VulkanDevice::Shutdown(VkAllocationCallbacks* allocator) {
		MZ_CORE_TRACE("Destroying device...");
		vkDestroyDevice(m_device, allocator);
	}
	
	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device)
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

			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices) {
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
	}
}