#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanPipeline {
	public:
		bool Create(VkRenderPass renderPass);
		void Destroy();
		void Bind(VkCommandBuffer commandBuffer);
		
		static VkVertexInputBindingDescription Vertex2dGetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> Vertex2dGetAttributeDescriptions();

		static 	VkVertexInputBindingDescription VulkanPipeline::Vertex3dGetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> Vertex3dGetAttributeDescriptions();
	
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		inline static const std::string s_engineMaterialShaderFragmentFileName = "assets/shaders/engine-material-shader.frag.spv";
		inline static const std::string s_engineMaterialShaderVertexFileName = "assets/shaders/engine-material-shader.vert.spv";
	};
}