#include "vulkan_command_buffer.h"

namespace mz {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandPool commandPool, VkDevice device)
	{
		m_commandPool = commandPool;
		m_device = device;
		m_handle = VK_NULL_HANDLE;
	}

	bool VulkanCommandBuffer::Create()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_device, &allocInfo, &m_handle) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to allocate command buffer!");
			return false;
		}
	}
	
	void VulkanCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;					// Optional
		beginInfo.pInheritanceInfo = nullptr;	// Optional

		VK_CHECK(vkBeginCommandBuffer(m_handle, &beginInfo));
	}
	
	void VulkanCommandBuffer::End()
	{
		VK_CHECK(vkEndCommandBuffer(m_handle));
	}
}
