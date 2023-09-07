#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/geometry.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanGeometry : public Geometry {
	public:
		VulkanGeometry(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices, std::string textureName);
		~VulkanGeometry();
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
		virtual void Draw(glm::mat4 model) const override;
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		uint32_t m_vertexCount;
		uint32_t m_vertexBufferOffset;

		uint32_t m_indexCount;
		uint32_t m_indexBufferOffset;

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;

		std::string m_textureName;
		Texture* m_texture;

		std::vector<VkDescriptorSet> m_descriptorSets;

		bool CreateVertexBuffer(std::vector<Vertex3d> vertices);
		bool CreateIndexBuffer(std::vector<uint32_t> indices);
		bool CreateDescriptorSets();
	};
}