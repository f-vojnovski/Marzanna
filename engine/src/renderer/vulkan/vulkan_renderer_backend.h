#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/renderer_backend.h"
#include "engine/src/core/window.h"
#include "vulkan_device.h"
#include "vulkan_swap_chain.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_pass.h"

namespace mz {
	class VulkanRendererBackend : public RendererBackend {
	public:
		VulkanRendererBackend(const RendererBackendArgs args);
		virtual bool Initialize() override;
		virtual void Shutdown() override;
		virtual bool BeginFrame() override;
		virtual bool EndFrame() override;
		virtual void OnResize() override;
		virtual void UpdateGlobalState(RendererGlobalState globalState) override;
		virtual void DrawGeometries(RendererGeometryData geometryData) override;
	private:
		bool m_isMinimized = false;
		std::shared_ptr<VulkanContext> contextPtr;
		VkDebugUtilsMessengerEXT m_debugMessegner;
		std::unique_ptr<VulkanDevice> m_device;
		std::shared_ptr<VulkanSwapChain> m_swapChain;
		std::unique_ptr<VulkanPipeline> m_pipeline;
		std::unique_ptr<VulkanRenderPass> m_mainRenderPass;

		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_types,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data);

		bool CreateCommandBuffers();
		bool CreateVertexBuffer();
		bool CreateIndexBuffer();
		bool CreateUniformBuffer();
		bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		bool CreateDescriptorSetLayout();
		bool CreateDescriptorPool();
		bool CreateDescriptorSets();
		void UpdateUniformObject(uint32_t currentImage);
	};
}