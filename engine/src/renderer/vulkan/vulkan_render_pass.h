#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_swap_chain.h"

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
	};
}