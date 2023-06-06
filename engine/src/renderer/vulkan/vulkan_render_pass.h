#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"
#include "vulkan_command_buffer.h"

namespace mz {
	class VulkanRenderPass {
	private:
		std::shared_ptr<VulkanSwapChain> m_swapChain;
		VkDevice m_device;

		VkRenderPass m_renderPass;
	public:
		VulkanRenderPass(VkDevice m_device, std::shared_ptr<VulkanSwapChain> swapChain);
		bool Create();
		void Destroy();
		void Begin(VulkanCommandBuffer& commandBuffer, uint32_t imageIndex);
		void End(VulkanCommandBuffer& commandBuffer, uint32_t imageIndex);
		inline VkRenderPass GetRenderPass() { return m_renderPass; }
	};
}