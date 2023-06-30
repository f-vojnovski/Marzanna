#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"
#include "vulkan_context.h"

namespace mz {
	class VulkanRenderPass {
	public:
		bool Create();
		void Destroy();
		void Begin(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void End(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;
	};
}