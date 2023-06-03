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
			if (IsDeviceSuitable(device)) {
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE) {
			MZ_CORE_CRITICAL("Failed to find suitable physical device!");
			return false;
		}

		MZ_CORE_INFO("Physical device selected!");
	}

	VulkanDevice::VulkanDevice()
	{
		m_physicalDevice = VK_NULL_HANDLE;
	}

	
	QueueFamilyIndices VulkanDevice::FindQueueFamilies()
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

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

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader;
	}
}