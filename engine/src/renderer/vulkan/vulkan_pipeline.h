#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"

namespace mz {
	class VulkanPipeline {
	private:
		inline static const std::string s_engineMaterialShaderFragmentFileName = "assets/shaders/engine-material-shader.frag.spv";
		inline static const std::string s_engineMaterialShaderVertexFileName = "assets/shaders/engine-material-shader.vert.spv";

		VkDevice m_device;
		std::shared_ptr<VulkanSwapChain> m_swapChain;

		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_graphicsPipeline;
	public:
		VulkanPipeline(VkDevice device, std::shared_ptr<VulkanSwapChain>);
		bool Create(VkRenderPass renderPass);
		void Destroy(VkAllocationCallbacks* allocator);
		void Bind(VkCommandBuffer commandBuffer);
		inline VkPipeline GetHandle() { return m_graphicsPipeline; }
	};
}