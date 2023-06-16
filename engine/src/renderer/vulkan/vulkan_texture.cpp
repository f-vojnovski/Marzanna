#include "vulkan_texture.h"

namespace mz {
	VulkanTexture::VulkanTexture(stbi_uc* pixels, int32_t width, int32_t height, int32_t channels) 
	{
		VkDeviceSize imageSize = width * height * channels;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		// Create a staging buffer in host visible memory so it can be usable as a transfer source for the image data
		VulkanFunctions::CreateBuffer(
			imageSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingBuffer, 
			stagingBufferMemory);

		// Copy values from image loading library to buffer
		void* data;
		vkMapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory);

		// Create the image for the texture
		VulkanFunctions::CreateImage(
			width, 
			height, 
			VK_FORMAT_R8G8B8A8_SRGB, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			image, 
			imageMemory);

		VulkanFunctions::TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanFunctions::CopyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		VulkanFunctions::TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(s_contextPtr->device.logicalDevice, stagingBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, s_contextPtr->allocator);
	}
	
	VulkanTexture::~VulkanTexture()
	{
		vkDestroyImage(s_contextPtr->device.logicalDevice, image, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, imageMemory, s_contextPtr->allocator);
	}
}