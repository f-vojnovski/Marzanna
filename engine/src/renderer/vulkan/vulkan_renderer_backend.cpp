#include "vulkan_renderer_backend.h"

namespace mz {
	VulkanRendererBackend::VulkanRendererBackend(const RendererBackendArgs args) 
	{
		m_name = args.name;
		m_window = args.window;
	}

	bool VulkanRendererBackend::Initialize()
	{
		return false;
	}
	
	void VulkanRendererBackend::Shutdown()
	{
	}
	
	bool VulkanRendererBackend::BeginFrame()
	{
		return false;
	}
	
	bool VulkanRendererBackend::EndFrame()
	{
		return false;
	}
}