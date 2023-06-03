#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/renderer_backend.h"
#include "vulkan_device.h"

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
		VulkanDevice m_device;
		bool CreateInstance();
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_types,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data);
	};
}