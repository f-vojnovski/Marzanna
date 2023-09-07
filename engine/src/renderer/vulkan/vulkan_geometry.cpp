#include "vulkan_geometry.h"
#include "vulkan_functions.h"

namespace mz {
	VulkanGeometry::VulkanGeometry(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices, std::string textureName)
	{
		m_vertexBufferOffset = s_contextPtr->vertexBufferOffset;
		m_vertexCount = vertices.size();

		m_indexBufferOffset = s_contextPtr->indexBufferOffset;
		m_indexCount = indices.size();

		CreateVertexBuffer(vertices);
		CreateIndexBuffer(indices);

		s_contextPtr->indexBufferOffset += indices.size();
		s_contextPtr->vertexBufferOffset += vertices.size();

		m_textureName = textureName;

		CreateDescriptorSets();
	}

	VulkanGeometry::~VulkanGeometry()
	{
		vkDeviceWaitIdle(s_contextPtr->device.logicalDevice);

		// Index buffer
		vkDestroyBuffer(s_contextPtr->device.logicalDevice, m_indexBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, m_indexBufferMemory, s_contextPtr->allocator);

		// Vertex buffer
		vkDestroyBuffer(s_contextPtr->device.logicalDevice, m_vertexBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, m_vertexBufferMemory, s_contextPtr->allocator);

		delete m_texture;
	}

	void VulkanGeometry::Draw(glm::mat4 model) const
	{
		VkCommandBuffer commandBuffer = s_contextPtr->commandBuffers[s_contextPtr->currentFrame];

		VkBuffer vertexBuffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		
		vkCmdPushConstants(commandBuffer, s_contextPtr->graphicsRenderingPipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &model);

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			s_contextPtr->graphicsRenderingPipeline.layout,
			0,
			1,
			&m_descriptorSets[s_contextPtr->currentFrame],
			0,
			nullptr);

		vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
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
			m_vertexBuffer,
			m_vertexBufferMemory)) {
			MZ_CORE_CRITICAL("Failed to create vertex buffer!");
			return false;
		}

		VulkanFunctions::CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

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
			m_indexBuffer,
			m_indexBufferMemory)) {
			MZ_CORE_CRITICAL("Failed to create index buffer!");
			return false;
		}

		VulkanFunctions::CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

		vkDestroyBuffer(s_contextPtr->device.logicalDevice, stagingBuffer, s_contextPtr->allocator);
		vkFreeMemory(s_contextPtr->device.logicalDevice, stagingBufferMemory, s_contextPtr->allocator);

		return true;
	}

	bool VulkanGeometry::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, s_contextPtr->graphicsRenderingPipeline.descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = s_contextPtr->graphicsRenderingPipeline.descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(s_contextPtr->device.logicalDevice, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
			return false;
		}

		// TODO: TEST CODE
		Texture::LoadTexture(m_textureName, &m_texture);
		VulkanTexture* vulkanTexturePtr = static_cast<VulkanTexture*>(m_texture);
		// TODO: END TEST CODE

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = s_contextPtr->uniformBuffers[i].handle;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanTexturePtr->GetImageView();
			imageInfo.sampler = s_contextPtr->textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(s_contextPtr->device.logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
		return true;
	}
}