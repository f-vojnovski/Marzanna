#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/renderer_backend.h"
#include "vulkan_device.h"
#include "vulkan_swap_chain.h"
#include "engine/src/core/window.h"

namespace mz {
	class VulkanRendererBackend : public RendererBackend {
	public:
		VulkanRendererBackend(const RendererBackendArgs args);
		virtual bool Initialize() override;
		virtual void Shutdown() override;
		virtual bool BeginFrame() override;
		virtual bool EndFrame() override;

	private:
		VkInstance m_instance;
		VkAllocationCallbacks* m_allocator;
		VkDebugUtilsMessengerEXT m_debugMessegner;
		std::unique_ptr<VulkanDevice> m_device;
		std::unique_ptr<VulkanSwapChain> m_swapChain;
		std::vector<const char*> m_validationLayers;
		VkSurfaceKHR m_surface;

		const Window* m_window;

		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_types,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data);
	};
}