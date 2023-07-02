#include "vulkan_geometry.h"
#include "vulkan_functions.h"

namespace mz {
	VulkanGeometry::VulkanGeometry(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices)
	{
		vertexBufferOffset = s_contextPtr->vertexBufferOffset;
		vertexCount = vertices.size();

		indexBufferOffset = s_contextPtr->indexBufferOffset;
		indexCount = indices.size();

		CreateVertexBuffer(vertices);
		CreateIndexBuffer(indices);

		s_contextPtr->indexBufferOffset += indices.size();
		s_contextPtr->vertexBufferOffset += vertices.size();
	}

	VulkanGeometry::~VulkanGeometry()
	{
		vkDeviceWaitIdle(s_contextPtr->device.logicalDevice);

		// Index buffer
		vkDestroyBuffer(s_contextPtr->device.logicalDevice, indexBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, indexBufferMemory, s_contextPtr->allocator);

		// Vertex buffer
		vkDestroyBuffer(s_contextPtr->device.logicalDevice, vertexBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, vertexBufferMemory, s_contextPtr->allocator);
	}

	void VulkanGeometry::Draw() const
	{
		VkCommandBuffer commandBuffer = s_contextPtr->commandBuffers[s_contextPtr->currentFrame];

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}
	
	bool VulkanGeometry::CreateVertexBuffer(std::vector<Vertex3d> vertices)
	{
		VkDeviceSize bufferSize = sizeof(Vertex3d) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		if (!VulkanFunctions::CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory)) {
			MZ_CORE_CRITICAL("Failed to create staging buffer!");
			return false;
		}


		void* data;
		vkMapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory);

		if (!VulkanFunctions::CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory)) {
			MZ_CORE_CRITICAL("Failed to create vertex buffer!");
			return false;
		}

		VulkanFunctions::CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(s_contextPtr->device.logicalDevice, stagingBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, s_contextPtr->allocator);
	}
	

	bool VulkanGeometry::CreateIndexBuffer(std::vector<uint32_t> indices)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanFunctions::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory);

		if (!VulkanFunctions::CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory)) {
			MZ_CORE_CRITICAL("Failed to create index buffer!");
			return false;
		}

		VulkanFunctions::CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(s_contextPtr->device.logicalDevice, stagingBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, s_contextPtr->allocator);

		return true;
	}
}