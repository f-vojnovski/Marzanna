#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanRenderPass {
	private:
		std::shared_ptr<VulkanContext> contextPtr;
	public:
		VulkanRenderPass(std::shared_ptr<VulkanContext> contextPtr);
		bool Create();
		void Destroy();
		void Begin(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void End(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	};
}