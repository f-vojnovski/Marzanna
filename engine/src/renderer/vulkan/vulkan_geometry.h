#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/geometry.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanGeometry : public Geometry {
	public:
		VulkanGeometry(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices);
		~VulkanGeometry();
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
		virtual void Draw(glm::mat4 model) const override;
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		uint32_t vertexCount;
		uint32_t vertexBufferOffset;

		uint32_t indexCount;
		uint32_t indexBufferOffset;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		bool CreateVertexBuffer(std::vector<Vertex3d> vertices);
		bool CreateIndexBuffer(std::vector<uint32_t> indices);
	};
}