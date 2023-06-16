#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanFunctions {
	public:
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
		
		static bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		static uint32_t FindDeviceMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);
		static bool CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		static VkCommandBuffer BeginSingleUseCommands();
		static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;
	};
}